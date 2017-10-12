/* (c) 2006-2006 faypou (a.k.a fc) */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma comment(lib, "libeay32.lib")

typedef unsigned __int64 uint64_t;

#else // _WIN32

#include <unistd.h>
#include <sys/time.h>

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef const char *   LPCSTR;
typedef unsigned long long uint64_t;

#endif // _WIN32

#include <openssl/des.h>

// ----------------------------------------------------------------------------

#define TRACE printf

#if 1
#define HIT_KEY_TO_CONTINUE() do { TRACE("\t\thit a key to continue\n");\
                                                                getc(stdin);\
                                                        }while (0)
#endif

//#define HIT_KEY_TO_CONTINUE()

// ----------------------------------------------------------------------------

#define SERIAL_LEN       (5)
#define ARGC_COUNT       (7)

#define MK         __argv[1]
#define DEL        __argv[2]
#define DKEY       __argv[3]
#define TDKEY      __argv[4]
#define OFFSET     __argv[5]
#define SERIAL     __argv[6]

#define TARGET     __argv[7]

// ----------------------------------------------------------------------------

typedef struct Digipass_GO3_ctx_
{
        BYTE  vMasterKey  [sizeof(DES_cblock) * 2];
        BYTE  vDEL        [sizeof(DES_cblock)    ];
        BYTE  vDES64KEY   [sizeof(DES_cblock)    ];
        BYTE  vA_TDES64KEY[sizeof(DES_cblock)    ];
        BYTE  vA_OFFSET   [sizeof(DES_cblock)    ];
        BYTE  vSERIAL     [SERIAL_LEN            ];

        // master keys
        DES_key_schedule  ks_master[2];

        // hold 3DES 112 "master-derived" keys
        DES_key_schedule  ks_digipass[2];
        DES_cblock        digipass_k[2];

        DES_cblock secret1; // 8 bytes
        DES_cblock secret2; // 8 bytes

        // only 3 first bytes used to derive OTPs
        DES_cblock        secret3;

        // finally, token keys
        DES_key_schedule ks_token[2];

} Digipass_GO3_ctx_t;

// ----------------------------------------------------------------------------
class CDigipassGO3
{
        public:
                CDigipassGO3() {
                        ResetState();
                        }

                ~CDigipassGO3() {
                        ResetState();
                        }

                static BYTE inline TO_I(char c) {
                        BYTE cc = (BYTE) toupper((BYTE)c);
                        return ( c > '9' /* hex chars */ ) ? (cc - '7') : (cc - '0');
                        }

                bool InitCtx(   LPCSTR szMK,
                                                LPCSTR szDEL,
                                                LPCSTR szDKEY,
                                                LPCSTR szTDKEY,
                                                LPCSTR szOFFSET,
                                                LPCSTR szSERIAL
                                                );

                void GetOTP(time_t start, char *GeneratedOTP = NULL);

                bool Synchronize(LPCSTR szTarget);

                time_t GetTimeDrift(void) {
                                return m_sync_delta;
                        }

                LPCSTR GetOTP_Str(void) {
                                return m_szTokenCode;
                        }

                enum {
                                // time step itself is fixed during Digipass init
                                TIME_WINDOW_SIZE = 100,

                                GO3_CODE_LEN = 6,

                                SEC_DELTA = 36,

                                GO3_PERIOD = (1 * SEC_DELTA)
                        };
 private:
                void ResetState(void) {
                        memset(this, 0, sizeof(*this));
                        }

                void MakePreSecretFromSerial(DES_cblock &pre, BYTE ord);

                bool DeriveKeys(void);

                static bool ConvertHexStrToByteVector(LPCSTR szSTR, BYTE *pBase);

                Digipass_GO3_ctx_t   m_ctx;
                BYTE                *m_pDerivePinPtr;
                size_t               m_sync_delta;
                char                 m_szTokenCode[GO3_CODE_LEN + 1];
};

// ----------------------------------------------------------------------------
// converts the hex string representation to byte vector representation;
// ----------------------------------------------------------------------------
bool CDigipassGO3::ConvertHexStrToByteVector(LPCSTR szSTR, BYTE *pBase)
{
        size_t len = strlen(szSTR);

        if ( len & 1 )
        {
                TRACE("\tmalformed hex_str not multiple of 2: '%s'...\n", szSTR);
                return false;
        }

        TRACE("\tconverting '%s' to bin...\n", szSTR);

        for ( size_t i = 0, j = 0; i < len; j++, i += 2 )
        {
                pBase[j] = (TO_I(szSTR[i]) << 4) + TO_I(szSTR[i + 1]);
        }

        return true;
}

// ----------------------------------------------------------------------------
//
// we received string material; make DigipassGO3 derivations;
//
// ----------------------------------------------------------------------------
bool CDigipassGO3::InitCtx(             LPCSTR szMK,
 LPCSTR szDEL,
                                                                LPCSTR szDKEY,
                                                                LPCSTR szTDKEY,
                                                                LPCSTR szOFFSET,
                                                                LPCSTR szSERIAL
                                                        )
{
        if (    !ConvertHexStrToByteVector( szMK,     m_ctx.vMasterKey   ) ||
                        !ConvertHexStrToByteVector( szDEL,    m_ctx.vDEL         ) ||
                        !ConvertHexStrToByteVector( szDKEY,   m_ctx.vDES64KEY    ) ||
                        !ConvertHexStrToByteVector( szTDKEY,  m_ctx.vA_TDES64KEY ) ||
                        !ConvertHexStrToByteVector( szOFFSET, m_ctx.vA_OFFSET    ) ||
                        !ConvertHexStrToByteVector( szSERIAL, m_ctx.vSERIAL      )
                )
        {
                TRACE("\tcannot get str material....\n");
                return false;
        }

        return DeriveKeys();
}

// ----------------------------------------------------------------------------
// prepare the secrets from token serial number; each one has hadcoded value;
// ----------------------------------------------------------------------------
void CDigipassGO3::MakePreSecretFromSerial(DES_cblock &pre, BYTE ord)
{
        memset(&pre, 0, sizeof(DES_cblock));

        BYTE *p = (BYTE *) &pre;

        memcpy(p + (sizeof(DES_cblock) -SERIAL_LEN), m_ctx.vSERIAL, SERIAL_LEN);

        if ( ord == 0x01 ) {
                p[0] = 0x01;
        }
        else if ( ord == 0x02 ) {
                p[1] = 0x10;
        }
        else if ( ord == 0x03 ) {
                p[1] = 0x01;
        }
}
// ----------------------------------------------------------------------------
// Here, the digipass derivation actually happens.
// ----------------------------------------------------------------------------
bool CDigipassGO3::DeriveKeys()
{
        DES_cblock des1;
                memcpy(&des1, m_ctx.vMasterKey, sizeof(DES_cblock));

        DES_cblock des2;
                memcpy(&des2, m_ctx.vMasterKey + sizeof(DES_cblock), sizeof(DES_cblock));

        DES_set_key_unchecked(&des1, &m_ctx.ks_master[0]);
        DES_set_key_unchecked(&des2, &m_ctx.ks_master[1]);

        DES_ecb3_encrypt((DES_cblock *)m_ctx.vDEL, &m_ctx.digipass_k[0],
                                        &m_ctx.ks_master[0], &m_ctx.ks_master[1], &m_ctx.ks_master[0],
                                        DES_ENCRYPT
                                        );

        DES_ecb3_encrypt(&m_ctx.digipass_k[0], &m_ctx.digipass_k[1],
                                        &m_ctx.ks_master[0], &m_ctx.ks_master[1], &m_ctx.ks_master[0],
                                        DES_ENCRYPT
                                        );

        DES_set_odd_parity(&m_ctx.digipass_k[0]);
        DES_set_odd_parity(&m_ctx.digipass_k[1]);

        DES_set_key_unchecked(&m_ctx.digipass_k[0], &m_ctx.ks_digipass[0]);
        DES_set_key_unchecked(&m_ctx.digipass_k[1], &m_ctx.ks_digipass[1]);

        //
        // ks_digipass[0] && ks_digipass[1]
        //
        DES_cblock pre1;
                MakePreSecretFromSerial(pre1, 0x01);

        DES_cblock a;
        DES_cblock b;

        DES_ecb3_encrypt(&pre1, &a,
                                &m_ctx.ks_digipass[0], &m_ctx.ks_digipass[1], &m_ctx.ks_digipass[0],
                                DES_ENCRYPT
                                );
 for ( int i = 0; i < sizeof(DES_cblock); i++ )
        {
                DES_cblock tmp;
                        memcpy(&tmp, (BYTE *)&a + i, sizeof(DES_cblock) -i);
                        memcpy((BYTE *)&tmp + sizeof(DES_cblock) -i, m_ctx.vDES64KEY, i);

                DES_ecb3_encrypt(&tmp, &b,
                                &m_ctx.ks_digipass[0], &m_ctx.ks_digipass[1], &m_ctx.ks_digipass[0],
                                DES_ENCRYPT
                                );

                BYTE al = *(BYTE *) &b;
                BYTE cl = m_ctx.vDES64KEY[i] ^ al;
                ((BYTE *)&m_ctx.secret1)[i] = cl;
        }

        DES_cblock pre2;
                MakePreSecretFromSerial(pre2, 0x02);

        DES_ecb3_encrypt(&pre2, &a,
                                &m_ctx.ks_digipass[0], &m_ctx.ks_digipass[1], &m_ctx.ks_digipass[0],
                                DES_ENCRYPT
                                );

        //
        // FIXME: each loop/round should be unified in a separate function;
        //
        for ( int i = 0; i < sizeof(DES_cblock); i++ )
        {
                DES_cblock tmp;
                        memcpy(&tmp, (BYTE *)&a + i, sizeof(DES_cblock) -i);
                        memcpy((BYTE *)&tmp + sizeof(DES_cblock) -i, m_ctx.vA_TDES64KEY, i);

                DES_ecb3_encrypt(&tmp, &b,
                                                &m_ctx.ks_digipass[0], &m_ctx.ks_digipass[1], &m_ctx.ks_digipass[0],
                                                 DES_ENCRYPT
                                                );

                BYTE al = *(BYTE *) &b;
                BYTE cl = m_ctx.vA_TDES64KEY[i] ^ al;
                ((BYTE *)&m_ctx.secret2)[i] = cl;
        }

        DES_cblock pre3;
                MakePreSecretFromSerial(pre3, 0x03);

        DES_ecb3_encrypt(&pre3, &a,
                                        &m_ctx.ks_digipass[0], &m_ctx.ks_digipass[1], &m_ctx.ks_digipass[0],
                                        DES_ENCRYPT
                                        );

        for ( int i = 0; i < sizeof(DES_cblock); i++ )
        {
                DES_cblock tmp;
                        memcpy(&tmp, (BYTE *)&a + i, sizeof(DES_cblock) -i);
                        memcpy((BYTE *)&tmp + sizeof(DES_cblock) -i, m_ctx.vA_OFFSET, i);

                DES_ecb3_encrypt(&tmp, &b,
                                                &m_ctx.ks_digipass[0], &m_ctx.ks_digipass[1], &m_ctx.ks_digipass[0],
                                                 DES_ENCRYPT
                                                );

                BYTE al = *(BYTE *) &b;
                BYTE cl = m_ctx.vA_OFFSET[i] ^ al;
                ((BYTE *)&m_ctx.secret3)[i] = cl;
        }

        DES_set_key_unchecked(&m_ctx.secret1, &m_ctx.ks_token[0]);
        DES_set_key_unchecked(&m_ctx.secret2, &m_ctx.ks_token[1]);

        m_pDerivePinPtr = (BYTE *)&m_ctx.secret3;

        TRACE("\tCDigipassGO3::DeriveKeys() done...\n");

        return true;
}
// ----------------------------------------------------------------------------
// THE generator; start must have been sync'ed before generation;
// FIXME: thread unsafe outside MS CRT
// ----------------------------------------------------------------------------
void CDigipassGO3::GetOTP(time_t start, char *szTokenCode)
{
        DES_cblock token_code = { 0 };
        struct tm time_tm     = *(gmtime(&start)); // here

        DWORD dwTmpCalc31 = (DWORD) (time_tm.tm_min * 60);

        dwTmpCalc31 += (DWORD) time_tm.tm_sec;

        uint64_t tmp = (uint64_t) dwTmpCalc31 * (uint64_t)0x38E38E39;

        tmp >>= 32;
        tmp >>= 3;


        BYTE calc1 = ((BYTE)(time_tm.tm_year / (TIME_WINDOW_SIZE / 10)) << 4) +
                                        (BYTE)(time_tm.tm_year % 0x0A);

        BYTE calc2 = ((BYTE)(time_tm.tm_hour / (TIME_WINDOW_SIZE / 10)) << 4) +
                                        (BYTE)(time_tm.tm_hour % 0x0A);

        BYTE calc3 = (((BYTE) tmp / 0x0A) * GO3_CODE_LEN) + (BYTE) tmp;

        BYTE calcA = ((BYTE)(time_tm.tm_mday / (TIME_WINDOW_SIZE / 10)) << 4) +
                                        (BYTE)(time_tm.tm_mday % 0x0A);

        time_tm.tm_mon++; // time_tm.tm_mon + 1; // ok
        BYTE calcB = ((BYTE)(time_tm.tm_mon / (TIME_WINDOW_SIZE / 10)) << 4) +
                                        (BYTE)(time_tm.tm_mon % 0x0A);

        // [0], [1], [2] - ok (secret3[0], secret3[1], secret3[2])
        m_pDerivePinPtr[3] = calc1;
        m_pDerivePinPtr[4] = calcB;
        m_pDerivePinPtr[5] = calcA;
        m_pDerivePinPtr[6] = calc2;
        m_pDerivePinPtr[7] = calc3;

DES_ecb3_encrypt(&m_ctx.secret3, &token_code,
                                                &m_ctx.ks_token[0], &m_ctx.ks_token[1], &m_ctx.ks_token[0],
                                                DES_ENCRYPT
                                        );

        //
        // extrated from fixed binary position
        //
        const static BYTE c_table[0x100] = {
                                                        0x92, 0x82, 0x55, 0x23, 0x90, 0x71, 0x22, 0x63,
                                                        0x37, 0x25, 0xFE, 0xFF, 0xFA, 0xFB, 0xFC, 0xFD,
                                                        0x59, 0x53, 0x06, 0x44, 0x79, 0x75, 0x88, 0x13,
                                                        0x64, 0x36, 0xEF, 0xEA, 0xEB, 0xEC, 0xED, 0xEE,
                                                        0x34, 0x46, 0x35, 0x21, 0x57, 0x27, 0x20, 0x65,
                                                        0x77, 0x03, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
                                                        0x10, 0x78, 0x81, 0x49, 0x84, 0x01, 0x32, 0x96,
                                                        0x11, 0x02, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xCA,
                                                        0x04, 0x24, 0x00, 0x54, 0x45, 0x72, 0x87, 0x09,
                                                        0x73, 0x83, 0xBC, 0xBD, 0xBE, 0xBF, 0xBA, 0xBB,
                                                        0x76, 0x98, 0x12, 0x42, 0x38, 0x33, 0x94, 0x05,
                                                        0x91, 0x86, 0xAD, 0xAE, 0xAF, 0xAA, 0xAB, 0xAC,
                                                        0x28, 0x39, 0x68, 0x47, 0x15, 0x56, 0x60, 0x17,
                                                        0x99, 0x07, 0x9E, 0x9F, 0x9A, 0x9B, 0x9C, 0x9D,
                                                        0x26, 0x18, 0x50, 0x74, 0x93, 0x89, 0x70, 0x61,
                                                        0x31, 0x58, 0x8F, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E,
                                                        0x16, 0x69, 0x30, 0x08, 0x43, 0x85, 0x67, 0x62,
                                                        0x95, 0x48, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
                                                        0x52, 0x66, 0x14, 0x29, 0x19, 0x97, 0x51, 0x40,
                                                        0x80, 0x41, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x6A,
                                                        0xE5, 0xF4, 0xA3, 0xB2, 0xC1, 0xD0, 0xE9, 0xF8,
                                                        0xA7, 0xB6, 0x5C, 0x5D, 0x5E, 0x5F, 0x5A, 0x5B,
                                                        0xF5, 0xA4, 0xB3, 0xC2, 0xD1, 0xE0, 0xF9, 0xA8,
                                                        0xB7, 0xC6, 0x4D, 0x4E, 0x4F, 0x4A, 0x4B, 0x4C,
                                                        0xA5, 0xB4, 0xC3, 0xD2, 0xE1, 0xF0, 0xA9, 0xB8,
                                                        0xC7, 0xD6, 0x3E, 0x3F, 0x3A, 0x3B, 0x3C, 0x3D,
                                                        0xB5, 0xC4, 0xD3, 0xE2, 0xF1, 0xA0, 0xB9, 0xC8,
                                                        0xD7, 0xE6, 0x2F, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E,
                                                        0xC5, 0xD4, 0xE3, 0xF2, 0xA1, 0xB0, 0xC9, 0xD8,
                                                        0xE7, 0xF6, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                                                        0xD5, 0xE4, 0xF3, 0xA2, 0xB1, 0xC0, 0xD9, 0xE8,
                                                        0xF7, 0xA6, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x0A
                                                };
 BYTE *pTokenCode  = (BYTE *) &token_code;
        BYTE *pTokenCode2 = pTokenCode;

        BYTE cl = pTokenCode[0];
        BYTE dl = pTokenCode[2];
        BYTE al = pTokenCode[3];
        BYTE bl = 0;

        dl ^= cl;

        cl            = pTokenCode[4];
        pTokenCode[2] = dl;
        dl            = pTokenCode[1];

        pTokenCode2 += 4;
        al ^= dl;

        dl            = pTokenCode[5];
        pTokenCode[3] = al;

        al = pTokenCode[6];
        cl ^= al;

        pTokenCode2[0] = cl;

        cl = pTokenCode[7];
        dl ^= cl;

        pTokenCode[5] = dl;

        for ( int i = 0; i < sizeof(DES_cblock); i++ )
        {
                al = pTokenCode[i];

                if ( al >= 0xA0 )
                {
                        al -= 0x60;
                }

                pTokenCode[i] = al;

                BYTE bl = al;

                bl &= 0x0F;

                if ( bl >= 0x0A )
                {
                        al -= 0x06;
                        pTokenCode[i] = al;
                }
        }

        dl = m_pDerivePinPtr[7];
        pTokenCode[6] = dl;
 for ( int i = 0; i < GO3_CODE_LEN; i++ )
        {
                for ( int j = 0; j < (GO3_CODE_LEN / 2); j++ )
                {
                        al = pTokenCode2[j];
                        dl = al;
                        al &= 0x0F;
                        dl >>= 4;

                        DWORD dwTmp1 = (DWORD) dl;
                        DWORD dwTmp2 = (DWORD) al;

                        dwTmp1 &= 0x000000FF;
                        dwTmp2 &= 0x000000FF;

                        dwTmp1 <<= 4;
                        al = c_table[dwTmp1 + dwTmp2];
                        pTokenCode2[j] = al;
                }

                dl = pTokenCode2[1];
                cl = pTokenCode2[2];

                bl = dl;
                al = cl;
                bl &= 0x0F;
                al &= 0x0F;

                bl <<= 4;
                cl >>= 4;
                bl += cl;

                cl =  pTokenCode2[0];
                pTokenCode2[2] = bl;

                bl = cl;
                bl &= 0x0F;
                bl <<= 4;
                dl >>= 4;
                cl >>= 4;
                al <<= 4;
                bl += dl;
                cl += al;

                pTokenCode2[1] = bl;
                pTokenCode2[0] = cl;
        }
 #endif

        //
        // optimized lookup convertion; see sprintf() disabled above;
        //
        const static char g_HexToStr[0x10] = {
                                                        '0', '1', '2', '3', '4',
                                                        '5', '6', '7', '8', '9',
                                                        //
                                                        // from now on, should never happen; they're
                                                        //  wrong if reached; the extra padding avoids
                                                        //  runtime "explosions";
                                                        //
                                                        'A', 'B', 'C', 'D','E', 'F'
                                                };

        //
        // loop unrolled, still for optimization purposes;
        //
        m_szTokenCode[0x00] = g_HexToStr[((pTokenCode2[0] >> 4) & 0x0F)];
        m_szTokenCode[0x01] = g_HexToStr[((pTokenCode2[0] >> 0) & 0x0F)];
        m_szTokenCode[0x02] = g_HexToStr[((pTokenCode2[1] >> 4) & 0x0F)];
        m_szTokenCode[0x03] = g_HexToStr[((pTokenCode2[1] >> 0) & 0x0F)];
        m_szTokenCode[0x04] = g_HexToStr[((pTokenCode2[2] >> 4) & 0x0F)];
        m_szTokenCode[0x05] = g_HexToStr[((pTokenCode2[2] >> 0) & 0x0F)];

        m_szTokenCode[0x06] = '\0';

        if ( szTokenCode != NULL )
                strcpy(szTokenCode, m_szTokenCode);
}
// ----------------------------------------------------------------------------
// Try to find time drift between token and localtime();
// This can be positive, or negative; depends on kind of time drift;
// Brute-force approach; FIXME
// ----------------------------------------------------------------------------
bool CDigipassGO3::Synchronize(LPCSTR szTarget)
{
        TRACE("\tSynchronize()ing with '%s'...\n", szTarget);


        time_t start = time(NULL);

        const size_t DAYS = 2 * (24 * 60 * 60); // 2 days in seconds

        TRACE("\t\tbackwards...\n");

        HIT_KEY_TO_CONTINUE();

        //
        // backwards
        //
        for ( m_sync_delta = 0; m_sync_delta < DAYS; m_sync_delta += GO3_PERIOD )
        {
                m_szTokenCode[0] = '\0';

                GetOTP(start - m_sync_delta);

                TRACE("\t\tround: %08u, %s:%s\n", m_sync_delta, szTarget, m_szTokenCode);

                if ( strcmp(szTarget, m_szTokenCode) == 0 )
                {
                        m_sync_delta = (~(DWORD)m_sync_delta) + 1; // negative, 2s-complement

                        TRACE("\t\tSynchronize() found negative drift!\n");
                        return true;
                }
        }

        TRACE("\t\tupwards...\n");

        HIT_KEY_TO_CONTINUE();

        //
        // upwards
        //
        for ( m_sync_delta = 0; m_sync_delta < DAYS; m_sync_delta += GO3_PERIOD )
        {
                m_szTokenCode[0] = '\0';

                GetOTP(start + m_sync_delta);

                TRACE("\t\tround: %08u, %s:%s\n", m_sync_delta, szTarget, m_szTokenCode);
  if ( strcmp(szTarget, m_szTokenCode) == 0 )
                {
                        TRACE("\t\tSynchronize() found positive drift!\n");
                        return true;
                }
        }

        return false;
}

// ----------------------------------------------------------------------------

#ifndef _WIN32
int    __argc;
char **__argv;
#endif // _WIN32

// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
#ifndef _WIN32
        __argc = argc;
        __argv = argv;
#endif // _WIN32

        if ( argc < ARGC_COUNT )
        {
                printf("\tincomplete arguments: MK DEL DKEY TDKEY OFFSET SERIAL [TARGET]\n");
                return -1;
        }

        bool bHasTarget = false;

        printf("\n");

        if ( argc == (ARGC_COUNT + 1) )
        {
                bHasTarget = true;
                printf("\t\tconvergence using '%s'...\n", TARGET);
        }


        CDigipassGO3 go3_token;

        if ( !go3_token.InitCtx(MK, DEL, DKEY, TDKEY, OFFSET, SERIAL) )
        {
                printf("\t\tcannot init token ctx...\n");
                return -2;
        }

  printf("\n");

        time_t start = time(NULL);

        if ( bHasTarget )
        {
                if ( !go3_token.Synchronize(TARGET) )
                {
                        printf("\t\tSynchronize() did not converge. aborted :(\n");
                        return -3;
                }
                else
                {
                        printf("\t\tdrif: 0x%08X...\n", go3_token.GetTimeDrift());

                        start += go3_token.GetTimeDrift();

                        HIT_KEY_TO_CONTINUE();
                }
        }

        int          round    = 0;
        const DWORD  WAIT_MSEC = 51;

        while ( true ) {

                char *str_time = ctime(&start);
                str_time[24] = '\0';

                go3_token.GetOTP(start);

                #if 0
                printf("\ttoken code ('%s':%03d): '%s'...\n", str_time,
                                                        round, go3_token.GetOTP_Str()
                                );
                #endif

                // for database manipulation
                printf("%d;%s\n", round, go3_token.GetOTP_Str());

                #if 0
                Sleep(WAIT_MSEC);
                #endif

                start += (CDigipassGO3::GO3_PERIOD);
                round++;

                if ( round > ((72000 + 10 + 2400) ) * 6) // ~ 6 month
                        break;
                //if ( start < 0 ) // time_t are long's in Win32; overflowed!
                //      break;
        }

        return 0;
}
// ----------------------------------------------------------------------------
