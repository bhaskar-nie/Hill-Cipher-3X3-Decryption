// hill_decrypt_crt_interactive.cpp
// 3x3 Hill cipher decryption using Chinese Remainder Theorem (mod 2 and mod 13 -> mod 26)
// Interactive: reads key and ciphertext from user input.
// Compile: g++ -std=c++17 -O2 hill_decrypt_crt_interactive.cpp -o hill_decrypt
// Run:   ./hill_decrypt
//
// Example interactive session:
// Enter 9-letter key (row-major): GYBNQKURP
// Enter ciphertext: POH
// Decrypted plaintext (uppercase): ACT

#include <bits/stdc++.h>
using namespace std;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int MOD_26 = 26;
const int MOD_2  = 2;
const int MOD_13 = 13;
using Matrix3x3 = array<array<int,3>,3>;

// ---------- Utility functions ----------
int letterIndex(char c) {
    return (int)ALPHABET.find(c);
}

string keepLettersUpper(const string &s) {
    string out;
    out.reserve(s.size());
    for (char ch : s) {
        if (isalpha((unsigned char)ch)) out.push_back((char)toupper((unsigned char)ch));
    }
    return out;
}

int positiveMod(int value, int mod) {
    int r = value % mod;
    if (r < 0) r += mod;
    return r;
}

// Extended Euclidean algorithm: returns gcd(a,b) and sets x,y so that a*x + b*y = gcd
long long extendedGcd(long long a, long long b, long long &x, long long &y) {
    if (b == 0) { x = 1; y = 0; return a; }
    long long x1, y1;
    long long g = extendedGcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

// Modular inverse of a modulo mod; returns -1 if inverse doesn't exist
int modularInverse(int a, int mod) {
    long long x, y;
    long long g = extendedGcd((a % mod + mod) % mod, mod, x, y);
    if (g != 1) return -1;
    return (int)((x % mod + mod) % mod);
}

// ---------- Matrix helpers (3x3) ----------
Matrix3x3 createKeyMatrixFromString(const string &keyString) {
    string cleaned = keepLettersUpper(keyString);
    if ((int)cleaned.size() != 9) {
        throw runtime_error("Key must contain exactly 9 alphabetic characters (A-Z).");
    }
    Matrix3x3 mat;
    for (int i = 0; i < 9; ++i) {
        int val = letterIndex(cleaned[i]);
        if (val < 0) throw runtime_error("Key contains invalid character.");
        mat[i/3][i%3] = val;
    }
    return mat;
}

int determinant3x3(const Matrix3x3 &m) {
    int a = m[0][0], b = m[0][1], c = m[0][2];
    int d = m[1][0], e = m[1][1], f = m[1][2];
    int g = m[2][0], h = m[2][1], i = m[2][2];
    int det = a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
    return det;
}

// Adjugate (transpose of cofactor matrix)
Matrix3x3 adjugate3x3(const Matrix3x3 &m) {
    Matrix3x3 adj;
    int a = m[0][0], b = m[0][1], c = m[0][2];
    int d = m[1][0], e = m[1][1], f = m[1][2];
    int g = m[2][0], h = m[2][1], i = m[2][2];

    int C00 =  (e*i - f*h);
    int C01 = -(d*i - f*g);
    int C02 =  (d*h - e*g);
    int C10 = -(b*i - c*h);
    int C11 =  (a*i - c*g);
    int C12 = -(a*h - b*g);
    int C20 =  (b*f - c*e);
    int C21 = -(a*f - c*d);
    int C22 =  (a*e - b*d);

    // adjugate = transpose of cofactor matrix
    adj[0][0] = C00; adj[0][1] = C10; adj[0][2] = C20;
    adj[1][0] = C01; adj[1][1] = C11; adj[1][2] = C21;
    adj[2][0] = C02; adj[2][1] = C12; adj[2][2] = C22;
    return adj;
}

Matrix3x3 scalarMultiplyMatrixMod(const Matrix3x3 &m, int scalar, int mod) {
    Matrix3x3 out;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            out[r][c] = positiveMod(1LL * positiveMod(m[r][c], mod) * (scalar % mod), mod);
    return out;
}

Matrix3x3 matrixMod(const Matrix3x3 &m, int mod) {
    Matrix3x3 out;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            out[r][c] = positiveMod(m[r][c], mod);
    return out;
}

// Combine residues r_mod2 (mod 2) and r_mod13 (mod 13) into value mod 26 using CRT.
// For modulus decomposition 26 = 2 * 13, precalculated CRT coefficients:
// x = 13 * r_mod2 + 14 * r_mod13  (mod 26)
int combineResiduesMod26(int residueMod2, int residueMod13) {
    int combined = (13 * (residueMod2 % MOD_2) + 14 * (residueMod13 % MOD_13)) % MOD_26;
    if (combined < 0) combined += MOD_26;
    return combined;
}

// Invert a 3x3 key matrix modulo 26 by inverting modulo 2 and modulo 13 separately and combining via CRT
Matrix3x3 invertKeyMatrixMod26UsingCrt(const Matrix3x3 &keyMatrix) {
    int det = determinant3x3(keyMatrix);
    Matrix3x3 adj = adjugate3x3(keyMatrix);

    int detMod2  = positiveMod(det, MOD_2);
    int detMod13 = positiveMod(det, MOD_13);

    if (detMod2 == 0)  throw runtime_error("Key matrix determinant is 0 modulo 2 -> not invertible mod 26.");
    if (detMod13 == 0) throw runtime_error("Key matrix determinant is 0 modulo 13 -> not invertible mod 26.");

    int detInverseMod2  = modularInverse(detMod2, MOD_2);
    int detInverseMod13 = modularInverse(detMod13, MOD_13);
    if (detInverseMod2 == -1 || detInverseMod13 == -1)
        throw runtime_error("Determinant not invertible modulo 2 or 13.");

    // adjugate reduced modulo 2 and modulo 13
    Matrix3x3 adjMod2  = matrixMod(adj, MOD_2);
    Matrix3x3 adjMod13 = matrixMod(adj, MOD_13);

    // inverse matrices modulo 2 and modulo 13
    Matrix3x3 inverseMod2  = scalarMultiplyMatrixMod(adjMod2, detInverseMod2, MOD_2);
    Matrix3x3 inverseMod13 = scalarMultiplyMatrixMod(adjMod13, detInverseMod13, MOD_13);

    // combine element-wise using CRT to get inverse modulo 26
    Matrix3x3 inverseMod26;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            int resid2  = positiveMod(inverseMod2[r][c], MOD_2);
            int resid13 = positiveMod(inverseMod13[r][c], MOD_13);
            inverseMod26[r][c] = combineResiduesMod26(resid2, resid13);
        }
    }
    return inverseMod26;
}

// Multiply 3x3 matrix by 3x1 vector modulo mod
array<int,3> multiplyMatrixVectorMod(const Matrix3x3 &matrix, const array<int,3> &vector, int mod) {
    array<int,3> result{};
    for (int r = 0; r < 3; ++r) {
        long long sum = 0;
        for (int c = 0; c < 3; ++c) sum += 1LL * matrix[r][c] * vector[c];
        result[r] = positiveMod(sum % mod, mod);
    }
    return result;
}

// ---------- Decryption ----------
string decryptCiphertextWithKeyInverse(const string &ciphertextInput, const Matrix3x3 &inverseKeyMatrix) {
    string cleanCipher = keepLettersUpper(ciphertextInput);
    // pad with 'X' to make length multiple of 3
    int paddingNeeded = (3 - (int)cleanCipher.size() % 3) % 3;
    cleanCipher.append(paddingNeeded, 'X');

    string plaintext;
    plaintext.reserve(cleanCipher.size());
    for (size_t i = 0; i < cleanCipher.size(); i += 3) {
        array<int,3> blockVector;
        for (int j = 0; j < 3; ++j) blockVector[j] = letterIndex(cleanCipher[i + j]);
        array<int,3> plainVector = multiplyMatrixVectorMod(inverseKeyMatrix, blockVector, MOD_26);
        for (int j = 0; j < 3; ++j) plaintext.push_back(ALPHABET[ plainVector[j] ]);
    }
    return plaintext;
}

// ---------- Main interactive routine ----------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    try {
        cout << "Enter 9-letter key (row-major, A-Z): ";
        string keyInput;
        if (!getline(cin, keyInput)) {
            cerr << "No key input provided.\n";
            return 1;
        }

        // Validate and build key matrix
        Matrix3x3 keyMatrix = createKeyMatrixFromString(keyInput);

        cout << "Enter ciphertext (any text; non-letters ignored): ";
        string ciphertextInput;
        if (!getline(cin, ciphertextInput)) {
            cerr << "No ciphertext input provided.\n";
            return 1;
        }

        // Compute inverse key matrix modulo 26 using CRT
        Matrix3x3 inverseKey = invertKeyMatrixMod26UsingCrt(keyMatrix);

        // Decrypt and print result
        string plaintext = decryptCiphertextWithKeyInverse(ciphertextInput, inverseKey);
        cout << "Decrypted plaintext (uppercase): " << plaintext << "\n";
    }
    catch (const exception &ex) {
        cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
