#ifndef K3MTABLES_H
#define K3MTABLES_H

class k3mTables
{
public:
    static int A1px[];
    static int A0[];
    static int A1[];
    static int A2[];
    static int A3[];
    static int A4[];
    static int A5[];
    static int* A[7];
};

int k3mTables::A1px[] = {49, 3, 6, 7, 12, 14, 15, 24, 28, 30, 31, 48, 56,
                     60, 62, 63, 96, 112, 120, 124, 126, 127, 129, 131,
                     135, 143, 159, 191, 192, 193, 195, 199, 207, 223,
                     224, 225, 227, 231, 239, 240, 241, 243, 247, 248,
                     249, 251, 252, 253, 254};

int k3mTables::A0[] = {49, 3, 6, 7, 12, 14, 15, 24, 28, 30, 31, 48, 56, 60,
                   62, 63, 96, 112, 120, 124, 126, 127, 129, 131, 135,
                   143, 159, 191, 192, 193, 195, 199, 207, 223, 224,
                   225, 227, 231, 239, 240, 241, 243, 247, 248, 249,
                   251, 252, 253, 254};

int k3mTables::A1[] = {9, 7, 14, 28, 56, 112, 131, 193, 224};

int k3mTables::A2[] = {17, 7, 14, 15, 28, 30, 56, 60, 112, 120, 131, 135,
                   193, 195, 224, 225, 240};

int k3mTables::A3[] = {25, 7, 14, 15, 28, 30, 31, 56, 60, 62, 112, 120,
                   124, 131, 135, 143, 193, 195, 199, 224, 225, 227,
                   240, 241, 248};

int k3mTables::A4[] = {33, 7, 14, 15, 28, 30, 31, 56, 60, 62, 63, 112, 120,
                   124, 126, 131, 135, 143, 159, 193, 195, 199, 207,
                   224, 225, 227, 231, 240, 241, 243, 248, 249, 252};

int k3mTables::A5[] = {37, 7, 14, 15, 28, 30, 31, 56, 60, 62, 63, 112, 120,
                   124, 126, 131, 135, 143, 159, 191, 193, 195, 199,
                   207, 224, 225, 227, 231, 239, 240, 241, 243, 248,
                   249, 251, 252, 254};

int* k3mTables::A[7] = {A0, A1, A2, A3, A4, A5, A1px};

#endif // K3MTABLES_H

