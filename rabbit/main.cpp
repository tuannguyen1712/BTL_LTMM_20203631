// eSTREAM Rabbit
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

uint32_t key[8];
uint32_t iv[2];                     // 64 bit IV
uint32_t c[8], oldc[8];             
uint32_t x[8];
uint32_t A[8] = {0x4D34D34D, 0xD34D34D3, 0x34D34D34, 0x4D34D34D, 
                0xD34D34D3, 0x34D34D34, 0x4D34D34D, 0xD34D34D3};
uint32_t g[8];
uint32_t carry;

uint32_t rotl32(uint32_t x, int n);
void key_setup();
void iv_setup();
void next_state();
void counter_system();
void encrypt(vector<uint32_t> plaintext);
void convert_to_bin(uint32_t val);

int main(int argc, char const *argv[])
{
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    string comment;
    getline(cin, comment);
    for (int i = 0; i < 8; i++)
        cin >> key[i];
    cin.ignore();
    getline(cin, comment);
    for (int i = 0; i < 2; i++)
        cin >> iv[i];
    cin.ignore();
    getline(cin, comment);
    vector<uint32_t> plaintext;
    int len;
    cin >> len;
    for (int i = 0; i < len; i++)
    {
        plaintext.push_back(0);
    }
    encrypt(plaintext);

    return 0;
}

uint32_t rotl32(uint32_t x, int n)
{
    uint32_t right = x >> (32 - n);
    uint32_t left = x << n;
    uint32_t result = right | left;
    return result;
}

void key_setup() {
    for (int i = 0; i < 8; i++) {
        if (i & 1)
        {
            x[i] = (key[(i + 5) & 7] << 16) | key[(i + 4) & 7];
            c[i] = (key[i] << 16) | (key[(i + 1) & 7]);
        }
        else
        {
            x[i] = (key[(i + 1) & 7] << 16) | key[i];
            c[i] = (key[(i + 4) & 7] << 16) | (key[(i + 5) & 7]);
        }
    }
    next_state();
    next_state();
    next_state();
    next_state();
    for (int i = 0; i < 8; i++)
        c[i] ^= x[(i + 4) & 7];
}

void iv_setup()
{
    c[0] ^= iv[0];
    c[2] ^= iv[1];
    c[4] ^= iv[0];
    c[6] ^= iv[1];
    c[1] ^= (((iv[1] >> 16) << 16) | ((iv[0] >> 16)));
    c[3] ^= ((iv[1] << 16) | ((iv[0] << 16) >> 16));
    c[5] ^= (((iv[1] >> 16) << 16) | ((iv[0] >> 16)));
    c[7] ^= ((iv[1] << 16) | ((iv[0] << 16) >> 16));
    next_state();
    next_state();
    next_state();
    next_state();
}

void encrypt(vector<uint32_t> plaintext)
{
    key_setup();
    iv_setup();
    vector<uint32_t> ciphertext;
    for (int i = 0; i < plaintext.size();)
    {
        next_state();
        for (int j = 0; j < 8 && i < plaintext.size(); j += 2, i++)
        {
            uint32_t temp = plaintext[i] ^ x[j] ^ (x[(j + 5) % 8] >> 16) ^ (x[(j + 3) % 8] << 16);
            ciphertext.push_back(temp);
            convert_to_bin(temp);
        }
    }
}

void counter_system()
{
    uint64_t temp;
    for (int i = 0; i < 8; i++)
        oldc[i] = c[i];

    temp = (c[0] & 0xFFFFFFFF) + (A[0] & 0xFFFFFFFF) + carry;
    c[0] = (uint32_t)(temp & 0xFFFFFFFF);

    for (int i = 1; i < 8; i++)
    {
        temp = (c[i] & 0xFFFFFFFF) + (A[i] & 0xFFFFFFFF) + (oldc[i - 1] > c[i - 1]);
        c[i] = (uint32_t)(temp & 0xFFFFFFFF);
    }
    carry = (oldc[7] > c[7]);
}

void next_state()
{
    counter_system();
    uint64_t temp;
    for (int i = 0; i < 8; i++)
    {
        temp = (x[i] + c[i]) & 0xFFFFFFFF;
        temp = temp * temp;
        g[i] = (uint32_t)(((temp) ^ (temp >> 32)) & 0xFFFFFFFF);
    }

    x[0] = g[0] + rotl32(g[7], 16) + rotl32(g[6], 16);
    x[1] = g[1] + rotl32(g[0], 8) + g[7];
    x[2] = g[2] + rotl32(g[1], 16) + rotl32(g[0], 16);
    x[3] = g[3] + rotl32(g[2], 8) + g[1];
    x[4] = g[4] + rotl32(g[3], 16) + rotl32(g[2], 16);
    x[5] = g[5] + rotl32(g[4], 8) + g[3];
    x[6] = g[6] + rotl32(g[5], 16) + rotl32(g[4], 16);
    x[7] = g[7] + rotl32(g[6], 8) + g[5];
}

void convert_to_bin(uint32_t val)
{
    string s = "00000000000000000000000000000000";
    int i = 31;
    while (val)
    {
        if (val & 1)
        {
            s[i] += 1;
        }
        val /= 2;
        i--;
    }
    cout << s << "\n";
}