#include "pch.h"
#include "../Classes/Engine/Engine.h"
#include "../Classes/Kismet/MemoryLibrary.h"
#include "../Classes/Engine/World.h"

#ifdef _MSC_VER
struct int128test{
    unsigned long long low;
    long long high;

    operator unsigned long long() const { return low; }
    operator bool() const { return low != 0 || high != 0; }
};
#endif

#define GET_BASE() InSDKUtils::GetImageBase()
#define REBASE_FUNC(Func) ((decltype(&Func))(GET_BASE() + 0x##Func))
#define REBASE_QWORD(Offset) (*(__int64*)(GET_BASE() + Offset))
#define REBASE_DWORD(Offset) (*(int*)(GET_BASE() + Offset))
#define REBASE_BYTE(Offset) (*(unsigned __int8*)(GET_BASE() + Offset))
#define REBASE_ARRAY(Type, Offset) ((Type*)(GET_BASE() + Offset))

typedef uint64_t _QWORD;
typedef uint32_t _DWORD;
typedef uint16_t _WORD;
typedef uint8_t  _BYTE;

#ifdef LOBYTE
#undef LOBYTE
#endif
#define LOBYTE(x) (*((_BYTE*)&(x)))
#define HIDWORD(x)  (*((_DWORD*)&(x) + 1))
#define WORD1(x)    (*((_WORD*)&(x) + 1))
#define SHIDWORD(x) (*((int32_t*)&(x) + 1))
#ifdef LODWORD
#undef LODWORD
#endif
#define LODWORD(x) (*((_DWORD*)&(x)))

struct _RTC_ALLOCA_NODE;

__int64 sub_11313CC(...); __int64 sub_13F9268(...); __int64 sub_1E6FAB0(...);
__int64 sub_F90FC8(...); __int64 sub_13FBCA0(...); __int64 sub_DDF264(...);
__int64 sub_143B200(...); __int64 sub_1135938(...); __int64 sub_D196F8(...);
__int64 sub_BCBEB0(...); __int64 sub_13FB0A8(...); __int64 sub_AFE310(...);
__int64 sub_BA100C(...); __int64 sub_DDE208(...); __int64 sub_22B9DA0(...);
__int64 sub_252A558(...); __int64 sub_13FBA4C(...); __int64 sub_24C8C88(...);
__int64 sub_20A021C(...); __int64 sub_D31954(...); __int64 sub_159A454(...);
__int64 sub_143AFE4(...); __int64 sub_1867804(...); __int64 sub_CD14B4(...);
__int64 sub_EB45D8(...); __int64 sub_14554DC(...); __int64 sub_1597CA4(...);
__int64 sub_1127A10(...); __int64 sub_11277C4(...); __int64 sub_B6A0A8(...);
__int64 sub_24116A4(...); __int64 sub_2571BF4(...); __int64 sub_13FB0EC(...);
__int64 sub_10DAA34(...); __int64 sub_1721E24(...); __int64 sub_13FB9AC(...);
__int64 sub_D693EC(...); __int64 sub_E84C7C(...); __int64 sub_1D9B1F8(...);
__int64 sub_BC90CC(...); __int64 sub_11E0C34(...); __int64 sub_6D98480(...);
__int64 sub_7A4C0B8(...); __int64 sub_D35F8C(...); __int64 sub_AFC728(...);
__int64 sub_E7CA58(...); __int64 sub_13F92BC(...); __int64 sub_11311A8(...);
__int64 sub_143E83C(...); __int64 sub_7A6EB4C(...); __int64 sub_B1ECB0(...);
__int64 sub_787EEB0(...); __int64 sub_D31A6C(...); __int64 sub_1C9E128(...);
__int64 sub_1C9DE70(...); __int64 sub_143E3A8(...); __int64 sub_DDD1E0(...);
__int64 sub_1D2AB44(...); __int64 sub_1439238(...); __int64 sub_7A70FB0(...);
__int64 sub_1439900(...); __int64 sub_1131004(...); __int64 sub_14078CC(...);
__int64 sub_1D9B170(...); __int64 sub_D39B38(...); __int64 sub_F11B38(...);
__int64 sub_7A48EC0(...); __int64 sub_7A4891C(...); __int64 sub_13F8B30(...);
__int64 sub_B6EDBC(...); __int64 sub_AF7834(...); __int64 sub_BCD120(...);
__int64 sub_BCD030(...); __int64 sub_79E7160(...); __int64 sub_13F8CE8(...);
__int64 sub_13F82F4(...); __int64 sub_143939C(...); __int64 sub_13F8D5C(...);
__int64 sub_13F90EC(...); __int64 sub_1406704(...); __int64 sub_3F2E260(...);
__int64 sub_C9EEBC(...); __int64 sub_7A48D88(...); __int64 sub_14063D4(...);
__int64 sub_1133AE4(...); __int64 sub_D75150(...); __int64 sub_13FB300(...);
__int64 sub_B669B0(...); __int64 sub_13F9224(...); __int64 sub_B00F38(...);

#define byte_B30CFA0  (*(uint8_t*)(GET_BASE() + 0xB30CFA0))
#define qword_B043CE0 (*(uint64_t*)(GET_BASE() + 0xB043CE0))
#define qword_B2AB040 (*(uint64_t*)(GET_BASE() + 0xB2AB040))
#define qword_9B9C938 (*(uint64_t*)(GET_BASE() + 0x9B9C938))
#define byte_B37C6C0  (*(uint8_t*)(GET_BASE() + 0xB37C6C0))
#define byte_A153CD0  ((uint8_t*)(GET_BASE() + 0xA153CD0))
#define dword_B57EDE0 (*(uint32_t*)(GET_BASE() + 0xB57EDE0))
#define qword_B57EDD8 (*(uint64_t*)(GET_BASE() + 0xB57EDD8))
#define qword_B0855D0 (*(uint64_t*)(GET_BASE() + 0xB0855D0))
#define qword_B318468 (*(uint64_t*)(GET_BASE() + 0xB318468))
#define qword_B3C9A88 (*(uint64_t*)(GET_BASE() + 0xB3C9A88))
#define qword_B3C9A78 (*(uint64_t*)(GET_BASE() + 0xB3C9A78))
#define dword_B318458 (*(uint32_t*)(GET_BASE() + 0xB318458))
#define qword_B452640 (*(uint64_t*)(GET_BASE() + 0xB452640))
#define dword_AE343D8 (*(uint32_t*)(GET_BASE() + 0xAE343D8))
#define qword_B043CB0 (*(uint64_t*)(GET_BASE() + 0xB043CB0))
#define qword_B421DB0 (*(uint64_t*)(GET_BASE() + 0xB421DB0))
#define byte_B30CFB5  (*(uint8_t*)(GET_BASE() + 0xB30CFB5))
#define qword_B59BD30 (*(uint64_t*)(GET_BASE() + 0xB59BD30))
#define byte_B37C650  (*(uint8_t*)(GET_BASE() + 0xB37C650))
#define byte_B30CF9F  (*(uint8_t*)(GET_BASE() + 0xB30CF9F))
#define byte_B417028  (*(uint8_t*)(GET_BASE() + 0xB417028))
#define qword_B6C4B18 (*(uint64_t*)(GET_BASE() + 0xB6C4B18))
#define asc_83E05EC   ((const wchar_t*)(GET_BASE() + 0x83E05EC))

inline wchar_t String;

bool __fastcall Engine::LoadMap(__int64 a1, unsigned __int8* a2, __int64 a3, wchar_t* a4, __int64 a5)
{
    const wchar_t* v5; // rax
    char v6; // r14
    __int64 v7; // rdi
    __int64 v9; // rsi
    __int64 v10; // rax
    wchar_t* v11; // r13
    __int64 v12; // rax
    __int64 v13; // r12
    __int64 v14; // rax
    __int64 v15 = 0; // rcx
    __int64 v16 = 0; // rcx
    __int64 v17 = 0; // rcx
    __int64 v18 = 0; // rcx
    __int64 v19 = 0; // rcx
    double v20; // xmm7_8
    __int64 v21; // r12
    __int64 v22; // rsi
    __int64 v23; // r11
    wchar_t* v24; // r9
    __int64 v25; // rdi
    const wchar_t* v26; // r10
    __int64 v27; // r8
    __int64 v28; // rdx
    __int64 v29; // r12
    __int64 v30; // rsi
    __int64 v31; // r11
    wchar_t* v32; // r9
    __int64 v33; // rdi
    const wchar_t* v34; // r10
    __int64 v35; // r8
    __int64 v36; // rdx
    wchar_t* v37; // rdx
    _BYTE* v38; // rbx
    __int64 v39; // rdx
    __int64 v40; // rsi
    int i; // edi
    __int64 v42; // rdx
    __int64 v43; // rcx
    int v44; // eax
    __int64 v45; // r8
    signed __int32 v46; // eax
    __int64 v47; // rdi
    int j; // ebx
    __int64 v49; // rdx
    _QWORD* v50; // rdi
    _QWORD* v51; // rsi
    void* v52; // rax
    struct _RTC_ALLOCA_NODE* v53; // rdx
    int v54; // r8d
    void* v55; // rbx
    __int64 v56; // rax
    __int64 v57; // rdi
    void(__fastcall * v58)(__int64, __int64); // rbx
    __int64 v59; // rax
    bool v60; // cc
    __int64 v61; // rdi
    wchar_t* v62; // rdx
    int v63; // edi
    __int64 v64; // rcx
    unsigned int v65; // ebx
    __int64 v66; // rax
    __int64 v67 = 0; // rcx
    wchar_t* v68; // rdx
    __int64 v69 = 0; // rcx
    __int64 v70; // rbx
    bool v71; // si
    int v72; // r8d
    wchar_t* v73; // rdx
    bool v74; // zf
    __int64 v75; // rax
    int v76; // eax
    __int64 v77; // r8
    __int64 v78; // rcx
    __int64 v79; // rbx
    signed __int32 v80; // eax
    __int64 v81; // rcx
    wchar_t* v82; // rdi
    __int64 v83; // rsi
    __int64 v84; // rbx
    __int64 v85 = 0; // r8
    __int64 v86; // rax
    __int64 v87; // r12
    __int64 v88; // rdi
    __int64 v89; // r10
    wchar_t* v90; // r8
    __int64 v91; // rsi
    const wchar_t* v92; // r11
    wchar_t* v93; // rdx
    __int64 v94; // r13
    int v95; // esi
    __int64 v96; // rax
    int v97; // edi
    const wchar_t* v98; // rbx
    wchar_t* v99; // rax
    __int64 v100; // r12
    int v101; // eax
    __int64 v102; // rsi
    __int64 v103; // rsi
    __int64 v104; // rsi
    wchar_t* v105; // r8
    const wchar_t* v106; // rax
    __int64 v107; // rdi
    __int64 v108; // rdx
    __int64 v109; // rsi
    __int64 v110; // rbx
    wchar_t* v111; // rdx
    _QWORD* v112; // rax
    __int64 v113; // rbx
    __int64 v114; // r8
    int v115; // edx
    int v116; // r9d
    __int64 v117; // r8
    __int64 v118; // r13
    int k; // r12d
    __int64 v120; // rax
    __int64 v121; // rcx
    __int64 v122 = 0; // rcx
    __int64 v123; // rcx
    int v124 = 0; // edx
    int v125 = 0; // ecx
    int v126 = 0; // r8d
    __int64 v127; // rbx
    void(__fastcall * v128)(__int64, __int64, _QWORD*); // rdi
    wchar_t* v129; // rax
    __int64 v130 = 0; // rdx
    __int64 v131; // rax
    __int64 v133; // rax
    __int64 v134; // rbx
    __int64 v135; // rsi
    __int64(__fastcall * v136)(__int64, __int64, wchar_t**, __int64); // rdi
    __int64 v137; // rax
    char v138; // al
    __int64 v139 = 0; // rdx
    __int64 v140; // rcx
    bool v141; // bl
    _QWORD* v142; // rbx
    __int64 v143; // rax
    __int64 v144 = 0; // rdx
    __int64 v145 = 0; // rcx
    __int64 v146 = 0; // r8
    wchar_t* v147; // rax
    wchar_t* v148; // rax
    __int64 v149; // rax
    __int64 v150; // r8
    __int64 v151; // rcx
    __int64 v152; // rdx
    __int64 v153; // rdi
    __int64 v154; // r10
    __int64 v155; // rsi
    const wchar_t* v156; // r11
    __int64 v157; // r9
    __int64 v158; // rdx
    __int64 v159; // r10
    __int64 v160; // rdi
    wchar_t* v161; // r8
    __int64 v162; // rsi
    const wchar_t* v163; // r11
    __int64 v164; // r9
    __int64 v165; // rdx
    __int64 v166; // r12
    __int64 v167; // rsi
    __int64 v168; // r11
    wchar_t* v169; // r9
    const wchar_t* v170; // r10
    __int64 v171; // rdi
    __int64 v172; // r8
    __int64 v173; // rdx
    __int64* v174; // rbx
    __int64* v175; // rsi
    __int64 v176; // rcx
    wchar_t* v177; // rdx
    __int64 v178; // rax
    wchar_t* v179; // rbx
    __int64 v180; // rax
    wchar_t* v181; // rdx
    _QWORD* v182; // rax
    __int64 v183; // rax
    __int64 v184; // rsi
    __int64 v185; // rax
    __int64 v186; // r9
    __int64 v187; // rdx
    __int64 v188; // rax
    __int64 v189 = 0; // rdx
    __int64 v190 = 0; // rcx
    __int64 v191 = 0; // r8
    wchar_t* v192; // rax
    int v193; // ecx
    int v194; // r12d
    int v195; // eax
    int v196; // ecx
    int v197; // eax
    __int64 v198; // rbx
    __int64 v199; // rdi
    wchar_t* v200; // rax
    __int64* v201; // r12
    __int64 v202; // r13
    __int64 v203; // rax
    __int64 v204; // rcx
    __int64 v205; // rdi
    void(__fastcall * v206)(__int64*, __int64, __int64, __int64, int128test*); // rsi
    __int64 v207; // rbx
    __int64 v208; // rax
    int v209; // eax
    __int64 v210; // [rsp+38h] [rbp-D0h] BYREF
    __int64 v211; // [rsp+40h] [rbp-C8h]
    __int64 v212; // [rsp+48h] [rbp-C0h]
    const wchar_t* v213; // [rsp+50h] [rbp-B8h] BYREF
    wchar_t* v214; // [rsp+58h] [rbp-B0h] BYREF
    __int64 v215 = 0;
    __int64 v215_arr[2] = { 0 };
    _QWORD v217[2]; // [rsp+70h] [rbp-98h] BYREF
    __int64 v218; // [rsp+80h] [rbp-88h] BYREF
    int128test v219; // [rsp+88h] [rbp-80h] BYREF
    _QWORD v220[2]; // [rsp+98h] [rbp-70h] BYREF
    _QWORD v221[2]; // [rsp+A8h] [rbp-60h] BYREF
    __int64 v222; // [rsp+B8h] [rbp-50h] BYREF
    LARGE_INTEGER v223; // [rsp+C0h] [rbp-48h] BYREF
    __int64 v224; // [rsp+C8h] [rbp-40h] BYREF
    wchar_t* v225; // [rsp+D0h] [rbp-38h] BYREF
    __int64 v226; // [rsp+D8h] [rbp-30h]
    LARGE_INTEGER PerformanceCount; // [rsp+E0h] [rbp-28h] BYREF
    _QWORD v228[2]; // [rsp+E8h] [rbp-20h] BYREF
    _QWORD v229[2]; // [rsp+F8h] [rbp-10h] BYREF
    wchar_t* v230; // [rsp+108h] [rbp+0h] BYREF
    int v231 = 0; // [rsp+110h] [rbp+8h]
    _QWORD v232[2]; // [rsp+118h] [rbp+10h] BYREF
    __int64 v233_struct[3] = {0};
    __int64* v236; // [rsp+140h] [rbp+38h] BYREF
    int128test v237; // [rsp+148h] [rbp+40h] BYREF
    char v238[8]; // [rsp+158h] [rbp+50h] BYREF
    __int64 v239; // [rsp+160h] [rbp+58h] BYREF
    __int64 v240; // [rsp+170h] [rbp+68h] BYREF
    __int64 v241; // [rsp+180h] [rbp+78h] BYREF
    __int64 v242; // [rsp+190h] [rbp+88h] BYREF
    __int64 v243; // [rsp+1A0h] [rbp+98h] BYREF
    char v244[104]; // [rsp+1B0h] [rbp+A8h] BYREF
    alignas(16) _BYTE v245_struct[64] = { 0 };
    __int64 d19_result_2 = 0;
    unsigned int global_dword = 0;
    __int64 global_qword = 0;

    v5 = L"STAT_LoadMap";
    v6 = 0;
    v214 = a4;
    v7 = a3;
    v211 = a3;
    if (byte_B30CFA0)
        v5 = 0;
    *(_QWORD*)&v219 = a1;
    v213 = v5;
    v9 = a1;
    v221[0] = a5;

    v10 = ((decltype(&sub_11313CC))(GET_BASE() + 0x11313CC))();
    ((decltype(&sub_13F9268))(GET_BASE() + 0x13F9268))(v10);
    ((decltype(&sub_1E6FAB0))(GET_BASE() + 0x1E6FAB0))();

    v11 = &String;
    ((decltype(&sub_F90FC8))(GET_BASE() + 0xF90FC8))(a5, &String);
    ((decltype(&sub_13FBCA0))(GET_BASE() + 0x13FBCA0))();

    v12 = *((_QWORD*)a2 + 78);
    if (v12)
        *(_BYTE*)(v12 + 275) &= ~0x80u;

    v13 = v7 + 40;
    v212 = v7 + 40;

    ((decltype(&sub_DDF264))(GET_BASE() + 0xDDF264))(&qword_B043CE0, v7 + 40);
    ((decltype(&sub_143B200))(GET_BASE() + 0x143B200))();
    ((decltype(&sub_1135938))(GET_BASE() + 0x1135938))();

    *(_DWORD*)(v9 + 3188) &= ~1u;
    v14 = *((_QWORD*)a2 + 78);
    if (v14)
    {
        if (*(_QWORD*)(v14 + 48))
        {
            __int64 arg_for_d19 = *(_QWORD*)(v14 + 48);
            __int64 d19_result = ((decltype(&sub_D196F8))(GET_BASE() + 0xD196F8))(arg_for_d19);
            if (d19_result != 0)
            {
                v215 = *(_QWORD*)(d19_result + 24);
            }
            else
            {
                v215 = 0;
            }
            ((decltype(&sub_BCBEB0))(GET_BASE() + 0xBCBEB0))(&v215, &v225);
            ((decltype(&sub_13FB0A8))(GET_BASE() + 0x13FB0A8))(v15, a2, 0, &v225);
            if (v225)
                ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v225);
        }
    }

    ((decltype(&sub_BA100C))(GET_BASE() + 0xBA100C))(v232, &String);
    ((decltype(&sub_13FB0A8))(GET_BASE() + 0x13FB0A8))(v16, a2, 1, v232);
    if (v232[0])
        ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v232[0]);

    ((decltype(&sub_BA100C))(GET_BASE() + 0xBA100C))(v229, &String);
    ((decltype(&sub_13FB0A8))(GET_BASE() + 0x13FB0A8))(v17, a2, 2, v229);
    if (v229[0])
        ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v229[0]);

    ((decltype(&sub_BA100C))(GET_BASE() + 0xBA100C))(v217, &String);
    ((decltype(&sub_13FB0A8))(GET_BASE() + 0x13FB0A8))(v18, a2, 4, v217);
    if (v217[0])
        ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v217[0]);

    ((decltype(&sub_DDE208))(GET_BASE() + 0xDDE208))(0xFFFFFFFFLL);
    ((decltype(&sub_22B9DA0))(GET_BASE() + 0x22B9DA0))(v19, a2);
    ((decltype(&sub_252A558))(GET_BASE() + 0x252A558))(a2 + 8);
    QueryPerformanceCounter(&PerformanceCount);
    v20 = (double)(int)PerformanceCount.LowPart * *(double*)&qword_B2AB040 + *(double*)&qword_9B9C938;

    if (byte_B37C6C0 >= 5u)
    {
        v143 = ((decltype(&sub_13FBA4C))(GET_BASE() + 0x13FBA4C))(v7, &v237, 0);
        v147 = *(_DWORD*)(v143 + 8) ? *(wchar_t**)v143 : &String;
        v223.QuadPart = (uintptr_t)v147;
        ((decltype(&sub_24C8C88))(GET_BASE() + 0x24C8C88))(v145, v144, v146, &v223);
        if ((_QWORD)v237)
            ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v237.low);
    }

    if (!*((_QWORD*)a2 + 78))
        goto LABEL_67;

    ((decltype(&sub_20A021C))(GET_BASE() + 0x20A021C))(*((_QWORD*)a2 + 78));

    if (*(int*)(v7 + 80) <= 0)
        goto LABEL_28;

    v21 = *(int*)(v7 + 80);
    v22 = 0;
    v23 = *(_QWORD*)(v7 + 72);
LABEL_19:
    if (*(_DWORD*)(v23 + 8))
        v24 = *(wchar_t**)v23;
    else
        v24 = &String;
    v25 = 5;
    v26 = L"quiet";
    while (1)
    {
        v27 = *(const wchar_t*)((uintptr_t)v26 + (uintptr_t)v24 - (uintptr_t)L"quiet");
        v28 = *v26++;
        if ((_WORD)v27 == (_WORD)v28)
        {
            if (!(_WORD)v27)
                goto LABEL_263;
            goto LABEL_24;
        }
        if ((((unsigned int)v27 | (unsigned int)v28) & 0xFFFFFF80) != 0)
            break;
        if (byte_A153CD0[v27] != byte_A153CD0[v28])
        {
        LABEL_236:
            ++v22;
            v23 += 16;
            if (v22 >= v21)
                goto LABEL_27;
            goto LABEL_19;
        }
    LABEL_24:
        if (!--v25)
            goto LABEL_263;
    }
    if ((_WORD)v27 != (_WORD)v28)
        goto LABEL_236;
LABEL_263:
    if (v24[4] != 61 && v24[5] != 61 && v24[5])
        goto LABEL_236;
    if (v24 != (wchar_t*)-10LL)
    {
        v38 = (_BYTE*)v219.low;
        goto LABEL_45;
    }
LABEL_27:
    v7 = v211;
    //v9 = v219.low;
    v13 = v212;
LABEL_28:
    *(_BYTE*)(v9 + 2249) = 2;
    ((decltype(&sub_D31954))(GET_BASE() + 0xD31954))(v9 + 2256, *(_QWORD*)v13, (*(_DWORD*)(v13 + 8) - 1) & (unsigned int)-(*(_DWORD*)(v13 + 8) != 0));
    if (*(int*)(v7 + 80) <= 0)
        goto LABEL_38;
    v29 = *(int*)(v7 + 80);
    v30 = 0;
    v31 = *(_QWORD*)(v7 + 72);
    while (2)
    {
        if (*(_DWORD*)(v31 + 8))
            v32 = *(wchar_t**)v31;
        else
            v32 = &String;
        v33 = 5;
        v34 = L"Game=";
        while (2)
        {
            v35 = *(const wchar_t*)((uintptr_t)v34 + (uintptr_t)v32 - (uintptr_t)L"Game=");
            v36 = *v34++;
            if ((_WORD)v35 == (_WORD)v36)
            {
                if (!(_WORD)v35)
                    goto LABEL_268;
            LABEL_35:
                if (!--v33)
                    goto LABEL_268;
                continue;
            }
            break;
        }
        if ((((unsigned int)v35 | (unsigned int)v36) & 0xFFFFFF80) == 0)
        {
            if (byte_A153CD0[v35] != byte_A153CD0[v36])
            {
            LABEL_239:
                ++v30;
                v31 += 16;
                if (v30 >= v29)
                    goto LABEL_38;
                continue;
            }
            goto LABEL_35;
        }
        break;
    }
    if ((_WORD)v35 != (_WORD)v36)
        goto LABEL_239;
LABEL_268:
    if (v32[4] != 61 && v32[5] != 61 && v32[5])
        goto LABEL_239;
    if (v32 == (wchar_t*)-10LL)
        goto LABEL_38;
    v167 = 0;
    v168 = *(_QWORD*)(v211 + 72);
    while (2)
    {
        if (*(_DWORD*)(v168 + 8))
            v169 = *(wchar_t**)v168;
        else
            v169 = &String;
        v170 = L"Game=";
        v171 = 5;
        while (2)
        {
            v172 = *(const wchar_t*)((uintptr_t)v170 + (uintptr_t)v169 - (uintptr_t)L"Game=");
            v173 = *v170++;
            if ((_WORD)v172 == (_WORD)v173)
            {
                if (!(_WORD)v172)
                    goto LABEL_285;
            LABEL_282:
                if (!--v171)
                    goto LABEL_285;
                continue;
            }
            break;
        }
        if ((((unsigned int)v172 | (unsigned int)v173) & 0xFFFFFF80) == 0)
        {
            if (byte_A153CD0[v172] != byte_A153CD0[v173])
                goto LABEL_288;
            goto LABEL_282;
        }
        if ((_WORD)v172 != (_WORD)v173)
            goto LABEL_288;
    LABEL_285:
        if (v169[4] == 61 || v169[5] == 61 || !v169[5])
        {
            v37 = v169 + 5;
            goto LABEL_39;
        }
    LABEL_288:
        ++v167;
        v168 += 16;
        if (v167 < *(int*)(v211 + 80))
            continue;
        break;
    }
LABEL_38:
    v37 = &String;
LABEL_39:
    v38 = (_BYTE*)a1;
    ((decltype(&sub_F90FC8))(GET_BASE() + 0xF90FC8))(a1 + 2272, v37);
    global_dword = *(unsigned int*)(GET_BASE() + 0xB57EDE0);
    global_qword = *(__int64*)(GET_BASE() + 0xB57EDD8);
    if (!global_dword
        || !global_qword
        || !(*(unsigned __int8(__fastcall**)(__int64))(*(_QWORD*)global_qword + 48LL))(global_qword)
        || !(*(unsigned __int8(__fastcall**)(__int64))(*(_QWORD*)global_qword + 72LL))(global_qword))
    {
        (*(void(__fastcall**)(_BYTE*))(*(_QWORD*)v38 + 1208LL))(v38);
    }
    v38[2249] = 0;
LABEL_45:
    ((decltype(&sub_159A454))(GET_BASE() + 0x159A454))(v38, *((_QWORD*)a2 + 78));
    LOBYTE(v39) = 2;
    ((decltype(&sub_143AFE4))(GET_BASE() + 0x143AFE4))(*((_QWORD*)a2 + 78), v39);
    ((decltype(&sub_1867804))(GET_BASE() + 0x1867804))(&qword_B0855D0, 0, *((_QWORD*)a2 + 78));
    v40 = *((_QWORD*)a2 + 65);
    if (v40)
    {
        for (i = 0; i >= 0 && i < *(_DWORD*)(v40 + 64); ++i)
        {
            v79 = *(_QWORD*)(*(_QWORD*)(v40 + 56) + 8LL * i);
            if (*(_QWORD*)(v79 + 48))
            {
                v149 = ((decltype(&sub_CD14B4))(GET_BASE() + 0xCD14B4))();
                v151 = *((_QWORD*)a2 + 78);
                if (v149 == v151)
                {
                    v152 = *(_QWORD*)(v79 + 48);
                    if (*(_QWORD*)(v152 + 608))
                    {
                        LOBYTE(v150) = 1;
                        ((decltype(&sub_EB45D8))(GET_BASE() + 0xEB45D8))(v151, *(_QWORD*)(v152 + 608), v150);
                        v152 = *(_QWORD*)(v79 + 48);
                        v151 = *((_QWORD*)a2 + 78);
                    }
                    LOBYTE(v150) = 1;
                    ((decltype(&sub_EB45D8))(GET_BASE() + 0xEB45D8))(v151, v152, v150);
                    *(_QWORD*)(v79 + 48) = 0;
                }
            }
            *(_DWORD*)(v79 + 160) &= ~1u;
            ((decltype(&sub_14554DC))(GET_BASE() + 0x14554DC))(v79);
        }
        v38 = (_BYTE*)v219.low;
    }
    ((void(__fastcall*)(void*, __int64, int))(GET_BASE() + 0x1597CA4))(v245_struct, *((_QWORD*)a2 + 78), 5);

    while (!v245_struct[36])
    {
        __int64 v246_val = *(__int64*)&v245_struct[48];

        ((void(__fastcall*)(__int64, int))(GET_BASE() + 0x1127A10))(v246_val, 1);

        ((void(__fastcall*)(void*))(GET_BASE() + 0x11277C4))(v245_struct);
    }

    ((void(__fastcall*)(void*))(GET_BASE() + 0xB6A0A8))(v245_struct);
    LOBYTE(v42) = 1;
    ((decltype(&sub_24116A4))(GET_BASE() + 0x24116A4))(*((_QWORD*)a2 + 78), v42);
    v43 = qword_B318468;
    if (qword_B318468)
    {
        if (*(_QWORD*)(qword_B318468 + 1952))
        {
            ((decltype(&sub_2571BF4))(GET_BASE() + 0x2571BF4))(v38);
            v43 = qword_B318468;
        }
        (*(void(__fastcall**)(__int64, _QWORD))(*(_QWORD*)v43 + 688LL))(v43, *((_QWORD*)a2 + 78));
    }
    v44 = *(_DWORD*)(*((_QWORD*)a2 + 78) + 12LL);
    if (v44 >= SHIDWORD(qword_B3C9A88))
        v45 = 0;
    else
        v45 = *(_QWORD*)(qword_B3C9A78 + 8LL * (v44 / 0x10000)) + 24LL * (v44 % 0x10000);
    do
        v46 = *(_DWORD*)(v45 + 8);
    while ((v46 & 0x40000000) != 0
        && v46 != _InterlockedCompareExchange((volatile long*)(v45 + 8), (long)(v46 & 0xBFFFFFFF), (long)v46));
    v47 = *((_QWORD*)a2 + 78);
    for (j = 0; j >= 0 && j < *(_DWORD*)(v47 + 336); ++j)
    {
        v78 = *(_QWORD*)(*(_QWORD*)(v47 + 328) + 8LL * j);
        if (v78)
            ((decltype(&sub_13FB0EC))(GET_BASE() + 0x13FB0EC))(*(_QWORD*)(v78 + 32));
    }
    v49 = *((_QWORD*)a2 + 78);
    v50 = *(_QWORD**)(v49 + 136);
    v51 = &v50[*(int*)(v49 + 144)];
    if (v50 != v51)
    {
        do
        {
            v142 = (_QWORD*)*v50;
            if (*(_QWORD*)(*v50 + 296LL)
                && (!(*(unsigned __int8(__fastcall**)(_QWORD))(*v142 + 712LL))(*v50)
                    || !(*(unsigned __int8(__fastcall**)(_QWORD*))(*v142 + 720LL))(v142)))
            {
                ((decltype(&sub_13FB0EC))(GET_BASE() + 0x13FB0EC))(*(_QWORD*)(v142[37] + 32LL));
            }
            ++v50;
        } while (v50 != v51);
        v49 = *((_QWORD*)a2 + 78);
    }
    v52 = (void*)((decltype(&sub_10DAA34))(GET_BASE() + 0x10DAA34))(v49);
    v55 = v52;
    if (v52)
    {
        LOBYTE(v54) = 1;
        // _RTC_AllocaFailure is likely a compiler intrinsic
        // _RTC_AllocaFailure(v52, v53, v54);
        ((decltype(&sub_1721E24))(GET_BASE() + 0x1721E24))(v55);
    }
    ((decltype(&sub_13FB9AC))(GET_BASE() + 0x13FB9AC))(a2, 0);
    v13 = v212;
    v9 = v219.low;
LABEL_67:
    ((decltype(&sub_143B200))(GET_BASE() + 0x143B200))();
    if (!dword_B318458)
        ((decltype(&sub_D693EC))(GET_BASE() + 0xD693EC))();
    if ((unsigned __int64)(qword_B452640 - 1) <= 0xFFFFFFFFFFFFFFFDuLL)
    {
        v56 = ((decltype(&sub_E84C7C))(GET_BASE() + 0xE84C7C))();
        (*(void(__fastcall**)(__int64))(*(_QWORD*)v56 + 40LL))(v56);
    }
    if (!dword_B318458)
        (*(void(__fastcall**)(__int64))(*(_QWORD*)v9 + 1256LL))(v9);
    ((decltype(&sub_143B200))(GET_BASE() + 0x143B200))();
    v57 = *((_QWORD*)a2 + 65);
    v58 = *(void(__fastcall**)(__int64, __int64))(*(_QWORD*)v57 + 880LL);
    v59 = ((decltype(&sub_1D9B1F8))(GET_BASE() + 0x1D9B1F8))((__int64)v244, v211);
    v58(v57, v59);
    v60 = *((_DWORD*)a2 + 140) <= 1;
    v61 = 0;
    v220[0] = 0;
    v218 = 0;
    if (!v60)
    {
        v174 = *(__int64**)(v9 + 3200);
        v175 = &v174[*(int*)(v9 + 3208)];
        while (v174 != v175)
        {
            v176 = *v174;
            if (*(_QWORD*)(*v174 + 624) != *((_QWORD*)a2 + 78) && *(int*)(v176 + 560) > 1)
            {
                v177 = *(_DWORD*)(v176 + 560) ? *(wchar_t**)(v176 + 552) : &String;
                if ((unsigned int)((decltype(&sub_BC90CC))(GET_BASE() + 0xBC90CC))(v13, (_DWORD)v177, 1, 0, -1) != -1)
                {
                    ((decltype(&sub_11E0C34))(GET_BASE() + 0x11E0C34))(&v230, v13);
                    dword_AE343D8 = *((_DWORD*)a2 + 136);
                    ((decltype(&sub_6D98480))(GET_BASE() + 0x6D98480))();
                    v218 = ((decltype(&sub_7A4C0B8))(GET_BASE() + 0x7A4C0B8))(a2, &v230, v220);
                    v61 = v218;
                    if (!v218)
                    {
                        v179 = v230;
                        if (v231)
                            v11 = v230;
                        ((decltype(&sub_D35F8C))(GET_BASE() + 0xD35F8C))(v217, L"Failed to load package '%s' while in PIE", v11);
                        ((decltype(&sub_AFC728))(GET_BASE() + 0xAFC728))(v221[0], v217);
                        if (v217[0])
                            ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v217[0]);
                        if (v179)
                            ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v179);
                        ((decltype(&sub_E7CA58))(GET_BASE() + 0xE7CA58))(&qword_B043CB0, 0);
                        v180 = ((decltype(&sub_11313CC))(GET_BASE() + 0x11313CC))();
                        ((decltype(&sub_13F92BC))(GET_BASE() + 0x13F92BC))(v180);
                        if (byte_B30CFA0 && v213)
                            ((decltype(&sub_11311A8))(GET_BASE() + 0x11311A8))(&v213);
                        ((decltype(&sub_143E83C))(GET_BASE() + 0x143E83C))(v211);
                        return 0;
                    }
                    v178 = ((decltype(&sub_7A6EB4C))(GET_BASE() + 0x7A6EB4C))(&v239, *((unsigned int*)a2 + 136));
                    ((decltype(&sub_AFC728))(GET_BASE() + 0xAFC728))(v61 + 192, v178);
                    if (v239)
                        ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v239);
                    byte_B30CFB5 = 1;
                    if (v230)
                        ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v230);
                }
            }
            ++v174;
        }
    }
    if (*a2 == 6)
    {
        if (*(_DWORD*)(v13 + 8))
            v181 = *(wchar_t**)v13;
        else
            v181 = &String;
        v182 = (_QWORD*)((decltype(&sub_B1ECB0))(GET_BASE() + 0xB1ECB0))(v238, v181, 1);
        ((decltype(&sub_787EEB0))(GET_BASE() + 0x787EEB0))(*v182, v220, &v218);
        v61 = v218;
    }
    ((decltype(&sub_D31A6C))(GET_BASE() + 0xD31A6C))(v228, v13);
    if (!v61)
    {
        if (*(_DWORD*)(v13 + 8))
            v62 = *(wchar_t**)v13;
        else
            v62 = &String;
        ((decltype(&sub_B1ECB0))(GET_BASE() + 0xB1ECB0))(&v224, v62, 1);
        v63 = *a2;
        v64 = ((unsigned __int16)v224 << 16) + HIDWORD(v224) + (unsigned __int16)v224 + 524289 * WORD1(v224);
        v65 = v64 + ((unsigned __int16)v224 >> 4);
        v66 = ((decltype(&sub_1C9E128))(GET_BASE() + 0x1C9E128))(v64, v65, &v224);
        if (!v66)
        {
            v236 = &v224;
            ((decltype(&sub_1C9DE70))(GET_BASE() + 0x1C9DE70))(v67, &v210, v65, &v236);
            v67 = 5LL * (int)v210;
            v66 = qword_B421DB0 + 20LL * (int)v210;
        }
        *(_DWORD*)(v66 + 8) = v63;
        if (*(_DWORD*)(v13 + 8))
            v68 = *(wchar_t**)v13;
        else
            v68 = &String;
        v220[0] = ((decltype(&sub_143E3A8))(GET_BASE() + 0x143E3A8))(v67, v68);
        v70 = v220[0];
        v71 = v220[0] != 0;
        if (!v220[0])
        {
            v72 = 0x80000;
            if (*a2 != 3)
                v72 = 0;
            if (*(_DWORD*)(v13 + 8))
                v73 = *(wchar_t**)v13;
            else
                v73 = &String;
            v70 = ((decltype(&sub_DDD1E0))(GET_BASE() + 0xDDD1E0))(0, (__int64)v73, v72, 0, 0);
            v220[0] = v70;
        }
        ((decltype(&sub_1D2AB44))(GET_BASE() + 0x1D2AB44))(v69, v224);
        if (!v70)
            goto LABEL_405;
        v218 = ((decltype(&sub_1439238))(GET_BASE() + 0x1439238))(v70);
        v61 = v218;
        if (v218)
            goto LABEL_90;
        v183 = ((decltype(&sub_7A70FB0))(GET_BASE() + 0x7A70FB0))(v70, 0);
        v218 = v183;
        v61 = v183;
        if (!v183)
        {
        LABEL_405:
            if (*(_DWORD*)(v13 + 8))
                v11 = *(wchar_t**)v13;
            ((decltype(&sub_D35F8C))(GET_BASE() + 0xD35F8C))(v217, L"Failed to load package '%s'", v11);
            ((decltype(&sub_AFC728))(GET_BASE() + 0xAFC728))(v221[0], v217);
            if (v217[0])
                ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v217[0]);
            goto LABEL_344;
        }
        v71 = 1;
        v70 = ((decltype(&sub_D196F8))(GET_BASE() + 0xD196F8))(v183);
        v220[0] = v70;
    LABEL_90:
        ((decltype(&sub_1439900))(GET_BASE() + 0x1439900))(*(_QWORD*)(v61 + 48));
        if (byte_B30CFA0)
            v70 = 0;
        v74 = *a2 == 3;
        v222 = v70;
        if (v74)
        {
            if (v71 || *(_BYTE*)(v61 + 274) == 2)
            {
                if (*((_DWORD*)a2 + 136) == -1)
                    *((_DWORD*)a2 + 136) = 0;
                v184 = v219.low;
                v61 = (*(__int64(__fastcall**)(_QWORD, unsigned __int8*, __int64, __int64))(*(_QWORD*)v219.low + 1000LL))(
                    v219.low,
                    a2,
                    v61,
                    v13);
                v218 = v61;
                byte_B30CFB5 = 1;
            }
            else
            {
                v184 = v219.low;
            }
            (*(void(__fastcall**)(__int64, __int64))(*(_QWORD*)v184 + 960LL))(v184, v61);
        }
        else if (*a2 == 1 && (*(_BYTE*)(v61 + 275) & 0x40) != 0 && *((_DWORD*)a2 + 136) != -1)
        {
            v218 = ((decltype(&sub_7A4C0B8))(GET_BASE() + 0x7A4C0B8))(a2, v13, v220);
            v61 = v218;
            if (!v218)
            {
                if (*(_DWORD*)(v13 + 8))
                    v11 = *(wchar_t**)v13;
                ((decltype(&sub_D35F8C))(GET_BASE() + 0xD35F8C))(v217, L"Failed to load package '%s' into a new game world.", v11);
                ((decltype(&sub_AFC728))(GET_BASE() + 0xAFC728))(v221[0], v217);
                if (v217[0])
                    ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v217[0]);
                if (byte_B30CFA0 && v70)
                    ((decltype(&sub_1131004))(GET_BASE() + 0x1131004))(&v222);
            LABEL_344:
                if (v228[0])
                    ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v228[0]);
                ((decltype(&sub_E7CA58))(GET_BASE() + 0xE7CA58))(&qword_B043CB0, 0);
                v185 = ((decltype(&sub_11313CC))(GET_BASE() + 0x11313CC))();
                ((decltype(&sub_13F92BC))(GET_BASE() + 0x13F92BC))(v185);
                if (byte_B30CFA0 && v213)
                    ((decltype(&sub_11311A8))(GET_BASE() + 0x11311A8))(&v213);
                goto LABEL_176;
            }
        }
        if (byte_B30CFA0 && v70)
            ((decltype(&sub_1131004))(GET_BASE() + 0x1131004))(&v222);
    }
    *(_QWORD*)(v61 + 400) = *((_QWORD*)a2 + 65);
    qword_B59BD30 = v61;
    ((decltype(&sub_13FB9AC))(GET_BASE() + 0x13FB9AC))(a2, v61);
    *(_BYTE*)(*((_QWORD*)a2 + 78) + 274LL) = *a2;
    v75 = *((_QWORD*)a2 + 78);
    if (*a2 == 3)
    {
        *(_DWORD*)(v75 + 8) &= ~2u;
    }
    else
    {
        v76 = *(_DWORD*)(v75 + 12);
        if (v76 < SHIDWORD(qword_B3C9A88))
            v77 = *(_QWORD*)(qword_B3C9A78 + 8LL * (v76 / 0x10000)) + 24LL * (v76 % 0x10000);
        else
            v77 = 0;
        do
            v80 = *(_DWORD*)(v77 + 8);
        while ((v80 & 0x40000000) == 0
            && v80 != _InterlockedCompareExchange((volatile long*)(v77 + 8), v80 | 0x40000000, v80));
    }
    v81 = *((_QWORD*)a2 + 78);
    if ((*(_BYTE*)(v81 + 275) & 0x40) == 0)
    {
        v221[1] = 0;
        LODWORD(v221[0]) = v221[0] & 0xFFFFF000 | 0x67F;
        v237.low = v221[0];
        v237.high = 0;
        ((decltype(&sub_14078CC))(GET_BASE() + 0x14078CC))(v81, &v237);
    }
    v82 = v214;
    if (v214)
        (*(void(__fastcall**)(_QWORD, unsigned __int8*))(*(_QWORD*)v219.low + 1304LL))(v219, a2);
    v83 = v211;
    ((decltype(&sub_1D9B170))(GET_BASE() + 0x1D9B170))(*((_QWORD*)a2 + 78), v211);
    v84 = ((decltype(&sub_10DAA34))(GET_BASE() + 0x10DAA34))(*((_QWORD*)a2 + 78));
    if (v84)
    {
        v86 = ((decltype(&sub_D39B38))(GET_BASE() + 0xD39B38))(v85, 0);
        if (v86)
        {
            ((decltype(&sub_F11B38))(GET_BASE() + 0xF11B38))(v84, *(_QWORD*)(v86 + 808));
        }
        else if ((unsigned __int8)byte_B37C650 >= 3u)
        {
            ((decltype(&sub_7A48EC0))(GET_BASE() + 0x7A48EC0))();
        }
    }
    if (!v82)
    {
        if (!byte_B30CF9F)
            goto LABEL_364;
        if (*(int*)(v83 + 80) > 0)
        {
            v87 = *(int*)(v83 + 80);
            v88 = 0;
            v89 = *(_QWORD*)(v83 + 72);
        LABEL_119:
            if (*(_DWORD*)(v89 + 8))
                v90 = *(wchar_t**)v89;
            else
                v90 = &String;
            v91 = 6;
            v92 = L"Listen";
            while (1)
            {
                v186 = *(const wchar_t*)((uintptr_t)v92 + (uintptr_t)v90 - (uintptr_t)L"Listen");
                v187 = *v92++;
                if ((_WORD)v186 == (_WORD)v187)
                {
                    if (!(_WORD)v186)
                        goto LABEL_359;
                }
                else
                {
                    if ((((unsigned int)v186 | (unsigned int)v187) & 0xFFFFFF80) != 0)
                    {
                        if ((_WORD)v186 != (_WORD)v187)
                            goto LABEL_260;
                    LABEL_359:
                        if (v90[5] != 61 && v90[6] != 61 && v90[6])
                            goto LABEL_260;
                        if (v90 == (wchar_t*)-12LL)
                            break;
                        v83 = v211;
                    LABEL_364:
                        bool bListen = World::Listen(*reinterpret_cast<UWorld**>(__int64(a2) + 0x270), *reinterpret_cast<FURL*>(v211));
                        if (!bListen)
                        {
                            if ((unsigned __int8)byte_B417028 >= 2u)
                            {
                                v188 = ((decltype(&sub_13FBA4C))(GET_BASE() + 0x13FBA4C))(v83, v232, 0);
                                v192 = *(_DWORD*)(v188 + 8) ? *(wchar_t**)v188 : &String;
                                v214 = v192;
                                ((decltype(&sub_7A4891C))(GET_BASE() + 0x7A4891C))(v190, v189, v191, &v214);
                                if (v232[0])
                                    ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v232[0]);
                            }
                        }

                        break;
                    }
                    if (byte_A153CD0[v186] != byte_A153CD0[v187])
                    {
                    LABEL_260:
                        ++v88;
                        v89 += 16;
                        if (v88 >= v87)
                            break;
                        goto LABEL_119;
                    }
                }
                if (!--v91)
                    goto LABEL_359;
            }
        }
    }
    if (*(int*)(v211 + 80) <= 0)
    {
    LABEL_124:
        v93 = &String;
        goto LABEL_125;
    }
    v153 = 0;
    v154 = *(_QWORD*)(v211 + 72);
    while (2)
    {
        if (*(_DWORD*)(v154 + 8))
            v105 = *(wchar_t**)v154;
        else
            v105 = &String;
        v155 = 8;
        v156 = L"Mutator=";
        while (2)
        {
            v157 = *(const wchar_t*)((uintptr_t)v156 + (uintptr_t)v105 - (uintptr_t)L"Mutator=");
            v158 = *v156++;
            if ((_WORD)v157 == (_WORD)v158)
            {
                if (!(_WORD)v157)
                    goto LABEL_371;
            LABEL_212:
                if (!--v155)
                    goto LABEL_371;
                continue;
            }
            break;
        }
        if ((((unsigned int)v157 | (unsigned int)v158) & 0xFFFFFF80) == 0)
        {
            if (byte_A153CD0[v157] != byte_A153CD0[v158])
            {
            LABEL_245:
                ++v153;
                v154 += 16;
                if (v153 >= *(int*)(v211 + 80))
                    goto LABEL_124;
                continue;
            }
            goto LABEL_212;
        }
        break;
    }
    if ((_WORD)v157 != (_WORD)v158)
        goto LABEL_245;
LABEL_371:
    if (v105[7] != 61 && v105[8] != 61 && v105[8])
        goto LABEL_245;
    v93 = v105 + 8;
    if (v105 != (wchar_t*)-16LL)
    {
    LABEL_125:
        v215_arr[0] = 0;
        v94 = 0;
        v215_arr[1] = 0;
        v95 = 0;
        v96 = ((__int64(__fastcall*)(__int64*, wchar_t*))((decltype(&sub_BA100C))(GET_BASE() + 0xBA100C)))(&v240, v93);
        v97 = 0;
        if (*(_DWORD*)(v96 + 8))
        {
            v98 = *(const wchar_t**)v96;
            if (!*(_QWORD*)v96)
            {
            LABEL_143:
                if (v240)
                    ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v240);
                if (v97 > 0)
                {
                    v198 = (unsigned int)v97;
                    v199 = v219.low;
                    do
                    {
                        ((decltype(&sub_13F8B30))(GET_BASE() + 0x13F8B30))(v199, *((_QWORD*)a2 + 78), 4, v94);
                        v94 += 16;
                        --v198;
                    } while (v198);
                }
                ((decltype(&sub_B6EDBC))(GET_BASE() + 0xB6EDBC))(&v215_arr);
                goto LABEL_147;
            }
        }
        else
        {
            v98 = &String;
        }
        if (*v98)
        {
            while (1)
            {
                v99 = const_cast<wchar_t*>(wcsstr(v98, asc_83E05EC));
                v222 = (__int64)v99;
                if (!v99)
                    break;
                v100 = v99 - v98;
                if (v100)
                {
                    v101 = v97++;
                    LODWORD(v210) = v101;
                    LODWORD(v215_arr[1]) = v97;
                    if (v97 > v95)
                    {
                        ((decltype(&sub_AF7834))(GET_BASE() + 0xAF7834))(&v215_arr);
                        v101 = v210;
                        v97 = v215_arr[1];
                        v94 = v215_arr[0];
                    }
                    v102 = v94 + 16LL * v101;
                    *(_QWORD*)v102 = 0;
                    *(_QWORD*)(v102 + 8) = 0;
                    if (v98 && (int)v100 > 0 && *v98)
                    {
                        ((decltype(&sub_BCD120))(GET_BASE() + 0xBCD120))(v94 + 16LL * v101, (unsigned int)(v100 + 1));
                        v193 = *(_DWORD*)(v102 + 8) + v100 + 1;
                        *(_DWORD*)(v102 + 8) = v193;
                        if (v193 > *(_DWORD*)(v102 + 12))
                            ((decltype(&sub_BCD030))(GET_BASE() + 0xBCD030))(v102);
                        memcpy(*(void**)v102, v98, 2LL * (int)v100);
                        *(_WORD*)(*(_QWORD*)v102 + 2LL * *(int*)(v102 + 8) - 2) = 0;
                    }
                    v95 = HIDWORD(v215_arr[1]);
                }
                v98 = (const wchar_t*)(v222 + 2);
            }
            if (*v98)
            {
                v103 = v97++;
                LODWORD(v215_arr[1]) = v97;
                if (v97 > SHIDWORD(v215_arr[1]))
                {
                    ((decltype(&sub_AF7834))(GET_BASE() + 0xAF7834))(&v215_arr);
                    v97 = (int)v215_arr[1];
                    v94 = v215_arr[0];
                }
                v104 = v94 + 16 * v103;
                *(_QWORD*)v104 = 0;
                *(_QWORD*)(v104 + 8) = 0;
                if (*v98)
                {
                    v166 = -1;
                    do
                        ++v166;
                    while (v98[v166]);
                    v194 = v166 + 1;
                    v195 = 0;
                    v196 = 0;
                    if (v194 > 0)
                    {
                        ((decltype(&sub_BCD120))(GET_BASE() + 0xBCD120))(v104, (unsigned int)v194);
                        v195 = *(_DWORD*)(v104 + 8);
                        v196 = *(_DWORD*)(v104 + 12);
                    }
                    v197 = v194 + v195;
                    *(_DWORD*)(v104 + 8) = v197;
                    if (v197 > v196)
                        ((decltype(&sub_BCD030))(GET_BASE() + 0xBCD030))(v104);
                    memcpy(*(void**)v104, v98, 2LL * v194);
                }
            }
        }
        goto LABEL_143;
    }
LABEL_147:
    if (qword_B6C4B18)
    {
        LOBYTE(v105) = 1;
        ((decltype(&sub_79E7160))(GET_BASE() + 0x79E7160))(qword_B6C4B18, 0, v105);
    }
    v106 = L"STAT_LoadMap_LoadPackagesFully";
    if (byte_B30CFA0)
        v106 = 0;
    v221[0] = (__int64)v106;
    d19_result_2 = ((decltype(&sub_D196F8))(GET_BASE() + 0xD196F8))(*(_QWORD*)(*((_QWORD*)a2 + 78) + 48LL));
    if (d19_result_2 != 0)
    {
        v214 = *(wchar_t**)(d19_result_2 + 24);
    }
    else
    {
        v214 = 0;
    }
    ((decltype(&sub_BCBEB0))(GET_BASE() + 0xBCBEB0))(&v214, v217);
    v107 = v219.low;
    ((decltype(&sub_13F8B30))(GET_BASE() + 0x13F8B30))(v107, *((_QWORD*)a2 + 78), 0, v217);
    if (v217[0])
    {
        ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v217[0]);
    }
    LOBYTE(v108) = 2;
    ((decltype(&sub_143AFE4))(GET_BASE() + 0x143AFE4))(*((_QWORD*)a2 + 78), v108);
    v109 = v212;
    v110 = *((_QWORD*)a2 + 78);
    if (*(_DWORD*)(v212 + 8))
        v111 = *(wchar_t**)v212;
    else
        v111 = &String;
    v112 = (_QWORD*)((decltype(&sub_B1ECB0))(GET_BASE() + 0xB1ECB0))(&PerformanceCount, v111, 1);
    ((decltype(&sub_13F8CE8))(GET_BASE() + 0x13F8CE8))(v110, *v112);
    if (byte_B30CFA0 && v221[0])
        ((decltype(&sub_11311A8))(GET_BASE() + 0x11311A8))(v221);
    ((decltype(&sub_143B200))(GET_BASE() + 0x143B200))();
    ((decltype(&sub_13F82F4))(GET_BASE() + 0x13F82F4))(*((_QWORD*)a2 + 78));
    v113 = v211;
    LOBYTE(v114) = 1;
    v233_struct[0] = *((_QWORD*)a2 + 78);
    v233_struct[1] = 0;
    v233_struct[2] = 0;
    ((decltype(&sub_143939C))(GET_BASE() + 0x143939C))(v233_struct[0], v211, v114, v233_struct);
    ((decltype(&sub_13F8D5C))(GET_BASE() + 0x13F8D5C))(v233_struct);
    if (v233_struct[1])
        ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v233_struct[1]);
    LOBYTE(v116) = 1;
    LOBYTE(v115) = 1;
    ((decltype(&sub_13F90EC))(GET_BASE() + 0x13F90EC))(*((_QWORD*)a2 + 78), v115, 0, v116, 0);
    ((decltype(&sub_1406704))(GET_BASE() + 0x1406704))(a2 + 192, v113);
    ((decltype(&sub_3F2E260))(GET_BASE() + 0x3F2E260))(a2 + 232, v228);
    if ((unsigned int)((decltype(&sub_C9EEBC))(GET_BASE() + 0xC9EEBC))(*((_QWORD*)a2 + 78)) == 3)
        ((decltype(&sub_1406704))(GET_BASE() + 0x1406704))(a2 + 296, v113);
    ((decltype(&sub_143B200))(GET_BASE() + 0x143B200))();
    v118 = *((_QWORD*)a2 + 65);
    if (v118)
    {
        for (k = 0; k >= 0 && k < *(_DWORD*)(v118 + 64); ++k)
        {
            v133 = *(_QWORD*)(v118 + 56);
            v134 = *((_QWORD*)a2 + 78);
            LOBYTE(v117) = 1;
            v225 = 0;
            v226 = 0;
            v135 = *(_QWORD*)(v133 + 8LL * k);
            v136 = *(__int64(__fastcall**)(__int64, __int64, wchar_t**, __int64))(*(_QWORD*)v135 + 728LL);
            v137 = ((decltype(&sub_13FBA4C))(GET_BASE() + 0x13FBA4C))(v211, &v241, v117);
            v138 = v136(v135, v137, &v225, v134);
            v140 = v241;
            v141 = v138 == 0;
            if (v241)
                ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v241);
            if (v141)
            {
                v200 = &String;
                if ((_DWORD)v226)
                    v200 = v225;
                v214 = v200;
                ((decltype(&sub_7A48D88))(GET_BASE() + 0x7A48D88))(v140, v139, v117, &v214);
            }
            if (v225)
                ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v225);
        }
        v109 = v212;
        v107 = v219.low;
        v113 = v211;
    }
    v120 = ((decltype(&sub_E84C7C))(GET_BASE() + 0xE84C7C))();
    (*(void(__fastcall**)(__int64))(*(_QWORD*)v120 + 48LL))(v120);
    if (qword_B318468)
    {
        v121 = *(_QWORD*)(qword_B318468 + 2896);
        if (v121)
            (*(void(__fastcall**)(__int64, unsigned __int8*))(*(_QWORD*)v121 + 352LL))(v121, a2);
    }
    ((decltype(&sub_14063D4))(GET_BASE() + 0x14063D4))(*((_QWORD*)a2 + 78));
    ((decltype(&sub_143B200))(GET_BASE() + 0x143B200))();
    ((decltype(&sub_E7CA58))(GET_BASE() + 0xE7CA58))(&qword_B043CB0, *((_QWORD*)a2 + 78));
    ((decltype(&sub_1133AE4))(GET_BASE() + 0x1133AE4))();
    *(_BYTE*)(*((_QWORD*)a2 + 78) + 275LL) |= 1u;
    (*(void(__fastcall**)(__int64, _QWORD))(*(_QWORD*)v107 + 1192LL))(v107, 0);
    ((decltype(&sub_E84C7C))(GET_BASE() + 0xE84C7C))();
    ((decltype(&sub_D75150))(GET_BASE() + 0xD75150))(v122, 1);
    v123 = *(_QWORD*)(*((_QWORD*)a2 + 78) + 296LL);
    if (v123 && (*(unsigned __int8(__fastcall**)(__int64))(*(_QWORD*)v123 + 2088LL))(v123) || *(int*)(v113 + 80) <= 0)
        goto LABEL_170;
    v159 = *(_QWORD*)(v113 + 72);
    v160 = 0;
    while (2)
    {
        if (*(_DWORD*)(v159 + 8))
            v161 = *(wchar_t**)v159;
        else
            v161 = &String;
        v162 = 7;
        v163 = L"DemoRec";
        while (2)
        {
            v164 = *(const wchar_t*)((uintptr_t)v163 + (uintptr_t)v161 - (uintptr_t)L"DemoRec");
            v165 = *v163++;
            if ((_WORD)v164 == (_WORD)v165)
            {
                if (!(_WORD)v164)
                    goto LABEL_391;
            LABEL_220:
                if (!--v162)
                    goto LABEL_391;
                continue;
            }
            break;
        }
        if ((((unsigned int)v164 | (unsigned int)v165) & 0xFFFFFF80) == 0)
        {
            if (byte_A153CD0[v164] != byte_A153CD0[v165])
                goto LABEL_251;
            goto LABEL_220;
        }
        if ((_WORD)v164 != (_WORD)v165)
            goto LABEL_251;
    LABEL_391:
        if (v161[6] == 61 || v161[7] == 61 || !v161[7])
        {
            if (v161 != (wchar_t*)-14LL)
            {
                v201 = (__int64*)*((_QWORD*)a2 + 65);
                if (v201)
                {
                    v202 = v211;
                    v203 = ((decltype(&sub_13FB300))(GET_BASE() + 0x13FB300))(v211, L"DemoRec=", 0);
                    v204 = *v201;
                    v205 = v203;
                    v219.low = 0;
                    v219.high = 0;
                    v206 = *(void(__fastcall**)(__int64*, __int64, __int64, __int64, int128test*))(v204 + 824);
                    v207 = ((decltype(&sub_B669B0))(GET_BASE() + 0xB669B0))(*((_QWORD*)a2 + 78), &v243);
                    v208 = ((__int64(__fastcall*)(__int64*, __int64))((decltype(&sub_BA100C))(GET_BASE() + 0xBA100C)))(&v242, v205);
                    v206(v201, v208, v207, v202 + 72, &v219);
                    if (v242)
                        ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v242);
                    if (v243)
                        ((decltype(&sub_AFE310))(GET_BASE() + 0xAFE310))(v243);
                }
            }
            break;
        }
    LABEL_251:
        ++v160;
        v159 += 16;
        if (v160 < *(int*)(v113 + 80))
            continue;
        break;
    }
    v109 = v212;
LABEL_170:
    QueryPerformanceCounter(&v223);
    if ((unsigned __int8)byte_B37C6C0 >= 5u)
    {
        if (*(_DWORD*)(v109 + 8))
            v148 = *(wchar_t**)v109;
        else
            v148 = &String;
        v214 = v148;
        *(double*)&v223.QuadPart = (double)(int)v223.LowPart * *(double*)&qword_B2AB040 + *(double*)&qword_9B9C938 - v20;
        ((decltype(&sub_13F9224))(GET_BASE() + 0x13F9224))(v125, v124, v126, (unsigned int)&v223, (__int64)&v214);
    }
    ((decltype(&sub_13FBCA0))(GET_BASE() + 0x13FBCA0))();
    v127 = *((_QWORD*)a2 + 65);
    v128 = *(void(__fastcall**)(__int64, __int64, _QWORD*))(*(_QWORD*)v127 + 760LL);
    if (*(_DWORD*)(v109 + 8))
        v129 = *(wchar_t**)v109;
    else
        v129 = &String;
    ((decltype(&sub_BA100C))(GET_BASE() + 0xBA100C))(v229, v129);
    v128(v127, v130, v229);
    ((decltype(&sub_B00F38))(GET_BASE() + 0xB00F38))(v229);
    if (dword_B318458)
    {
        ((decltype(&sub_D693EC))(GET_BASE() + 0xD693EC))();
        v209 = dword_B318458;
        if (dword_B318458 == 1)
            v209 = 0;
        dword_B318458 = v209;
    }
    ((decltype(&sub_B00F38))(GET_BASE() + 0xB00F38))(v228);
    v131 = ((decltype(&sub_11313CC))(GET_BASE() + 0x11313CC))();
    ((decltype(&sub_13F92BC))(GET_BASE() + 0x13F92BC))(v131);
    if (byte_B30CFA0 && v213)
        ((decltype(&sub_11311A8))(GET_BASE() + 0x11311A8))(&v213);
    v6 = 1;
LABEL_176:
    ((decltype(&sub_143E83C))(GET_BASE() + 0x143E83C))(v211);
    return v6;
}

void Engine::LoadModule()
{
    FMemoryLibrary::DetourHook(0x13f9ec8, LoadMap);
}