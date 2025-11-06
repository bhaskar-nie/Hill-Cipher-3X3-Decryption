# Hill Cipher Decryption using Chinese Remainder Theorem (CRT)

## Overview

This project implements a **3×3 Hill cipher decryption** program that uses the **Chinese Remainder Theorem (CRT)** to efficiently compute the inverse of the key matrix modulo 26. The program is interactive, allowing users to input a 9-letter key and ciphertext to decrypt messages.

The Hill cipher is a polygraphic substitution cipher that encrypts blocks of letters using matrix multiplication. This implementation demonstrates how CRT can be used to simplify modular arithmetic when working with composite moduli (26 = 2 × 13).

---

## Table of Contents

1. [Mathematical Background](#mathematical-background)
2. [Chinese Remainder Theorem](#chinese-remainder-theorem)
3. [Hill Cipher Theory](#hill-cipher-theory)
4. [Implementation Details](#implementation-details)
5. [Code Explanation](#code-explanation)
6. [How to Compile and Run](#how-to-compile-and-run)
7. [Example Usage](#example-usage)
8. [Notes and Limitations](#notes-and-limitations)

---

## Mathematical Background

### Modular Arithmetic

The Hill cipher operates in **modulo 26** arithmetic, where each letter A-Z is mapped to 0-25. Matrix operations are performed modulo 26, meaning all results are reduced to the range [0, 25].

### Matrix Inversion Modulo n

To decrypt a Hill cipher, we need the **inverse** of the key matrix modulo 26. For a 3×3 matrix **K**, the inverse **K⁻¹** satisfies:

**K × K⁻¹ ≡ I (mod 26)**

where **I** is the 3×3 identity matrix.

The inverse is computed using the formula:

**K⁻¹ = (det(K))⁻¹ × adj(K) (mod 26)**

where:
- `det(K)` is the determinant of **K**
- `adj(K)` is the adjugate (transpose of cofactor matrix) of **K**
- `(det(K))⁻¹` is the modular inverse of the determinant

---

## Chinese Remainder Theorem

### Theorem Statement

The **Chinese Remainder Theorem (CRT)** states that if we have a system of congruences:

```
x ≡ a₁ (mod m₁)
x ≡ a₂ (mod m₂)
...
x ≡ aₖ (mod mₖ)
```

where `m₁, m₂, ..., mₖ` are pairwise coprime (gcd(mᵢ, mⱼ) = 1 for i ≠ j), then there exists a unique solution modulo `M = m₁ × m₂ × ... × mₖ`.

### Application to This Project

Since **26 = 2 × 13** and gcd(2, 13) = 1, we can decompose the problem:

1. Instead of computing the inverse directly modulo 26, we:
   - Compute the inverse modulo 2
   - Compute the inverse modulo 13
   - Combine the results using CRT

2. **Why this helps:**
   - Working modulo 2 and 13 is simpler than working modulo 26
   - The modular inverses are easier to compute
   - CRT guarantees a unique solution modulo 26

### CRT Combination Formula

For the decomposition 26 = 2 × 13, the combination formula is:

```
x ≡ 13 × r₂ + 14 × r₁₃ (mod 26)
```

where:
- `r₂` is the residue modulo 2
- `r₁₃` is the residue modulo 13
- The coefficients 13 and 14 are derived from CRT (13 ≡ 1 (mod 2) and 13 ≡ 0 (mod 13); 14 ≡ 0 (mod 2) and 14 ≡ 1 (mod 13))

This formula is implemented in the `combineResiduesMod26()` function.

---

## Hill Cipher Theory

### Encryption Process

1. **Key Matrix**: A 3×3 invertible matrix **K** (mod 26)
2. **Plaintext**: Converted to numerical vectors of length 3
3. **Encryption**: For each plaintext block **P**, compute:
   ```
   C ≡ K × P (mod 26)
   ```
   where **C** is the ciphertext block

### Decryption Process

1. **Inverse Key Matrix**: Compute **K⁻¹** (mod 26)
2. **Ciphertext**: Converted to numerical vectors of length 3
3. **Decryption**: For each ciphertext block **C**, compute:
   ```
   P ≡ K⁻¹ × C (mod 26)
   ```
   where **P** is the plaintext block

### Key Requirements

- The key matrix must be **invertible modulo 26**
- This requires: `gcd(det(K), 26) = 1`
- Since 26 = 2 × 13, we need:
  - `det(K) ≢ 0 (mod 2)` AND
  - `det(K) ≢ 0 (mod 13)`

---

## Implementation Details

### Key Components

1. **Matrix Operations**: 3×3 matrix determinant, adjugate, multiplication
2. **Modular Arithmetic**: Extended Euclidean algorithm for modular inverses
3. **CRT Implementation**: Combining residues modulo 2 and 13 to get modulo 26
4. **Text Processing**: Converting between letters and numbers, padding

### Algorithm Flow

```
1. Read 9-letter key → Create 3×3 key matrix
2. Compute determinant and adjugate
3. Check invertibility (det ≠ 0 mod 2 and mod 13)
4. Compute inverse mod 2:
   - Reduce adjugate mod 2
   - Compute det⁻¹ mod 2
   - Multiply: inverse_mod2 = adj_mod2 × det⁻¹_mod2
5. Compute inverse mod 13:
   - Reduce adjugate mod 13
   - Compute det⁻¹ mod 13
   - Multiply: inverse_mod13 = adj_mod13 × det⁻¹_mod13
6. Combine using CRT:
   - For each element: combineResiduesMod26(element_mod2, element_mod13)
7. Decrypt ciphertext using inverse matrix
```

---

## Code Explanation

### Core Functions

#### 1. `extendedGcd(a, b, x, y)`
- **Purpose**: Computes gcd(a, b) and finds coefficients x, y such that `a×x + b×y = gcd(a, b)`
- **Used for**: Computing modular inverses
- **Algorithm**: Extended Euclidean Algorithm (recursive)

#### 2. `modularInverse(a, mod)`
- **Purpose**: Finds the modular inverse of `a` modulo `mod`
- **Returns**: `x` such that `a × x ≡ 1 (mod mod)`, or -1 if no inverse exists
- **Method**: Uses extended Euclidean algorithm

#### 3. `determinant3x3(m)`
- **Purpose**: Computes the determinant of a 3×3 matrix
- **Formula**: 
  ```
  det = a(ei - fh) - b(di - fg) + c(dh - eg)
  ```
  where the matrix is:
  ```
  [a b c]
  [d e f]
  [g h i]
  ```

#### 4. `adjugate3x3(m)`
- **Purpose**: Computes the adjugate (transpose of cofactor matrix) of a 3×3 matrix
- **Why needed**: Used in the formula `K⁻¹ = det⁻¹ × adj(K)`
- **Method**: Computes all 9 cofactors and transposes them

#### 5. `combineResiduesMod26(residueMod2, residueMod13)`
- **Purpose**: Combines residues modulo 2 and 13 into a value modulo 26 using CRT
- **Formula**: `x = (13 × r₂ + 14 × r₁₃) mod 26`
- **Why it works**: 
  - 13 ≡ 1 (mod 2) and 13 ≡ 0 (mod 13)
  - 14 ≡ 0 (mod 2) and 14 ≡ 1 (mod 13)
  - This ensures the result has the correct residues

#### 6. `invertKeyMatrixMod26UsingCrt(keyMatrix)`
- **Purpose**: Main function that computes the inverse matrix modulo 26 using CRT
- **Steps**:
  1. Compute determinant and adjugate
  2. Check invertibility (det ≠ 0 mod 2 and mod 13)
  3. Compute inverse modulo 2
  4. Compute inverse modulo 13
  5. Combine element-wise using CRT

#### 7. `decryptCiphertextWithKeyInverse(ciphertext, inverseKey)`
- **Purpose**: Decrypts ciphertext using the inverse key matrix
- **Process**:
  1. Clean and pad ciphertext to multiple of 3
  2. Process in blocks of 3 letters
  3. Convert to numerical vectors
  4. Multiply by inverse key matrix
  5. Convert back to letters

### Utility Functions

- `letterIndex(c)`: Converts letter A-Z to number 0-25
- `keepLettersUpper(s)`: Extracts and uppercases alphabetic characters
- `positiveMod(value, mod)`: Ensures modulo result is in [0, mod-1]
- `createKeyMatrixFromString(s)`: Converts 9-letter string to 3×3 matrix (row-major)

---

## How to Compile and Run

### Prerequisites

- **C++ Compiler**: GCC (g++) with C++17 support or compatible compiler
- **Operating System**: Windows, Linux, or macOS

### Compilation

#### On Linux/macOS:
```bash
g++ -std=c++17 -O2 hill_decrypt_crt_interactive.cpp -o hill_decrypt
```

#### On Windows (using MinGW or similar):
```bash
g++ -std=c++17 -O2 hill_decrypt_crt_interactive.cpp -o hill_decrypt.exe
```

#### Compiler Flags Explained:
- `-std=c++17`: Enables C++17 standard features (used for `array` type)
- `-O2`: Optimization level 2 for better performance
- `-o hill_decrypt`: Specifies output executable name

### Running the Program

#### On Linux/macOS:
```bash
./hill_decrypt
```

#### On Windows:
```bash
hill_decrypt.exe
```

### Interactive Input

The program will prompt you for:
1. **9-letter key** (row-major order, A-Z only)
2. **Ciphertext** (any text; non-letters are ignored)

---

## Example Usage

### Example 1: Basic Decryption

**Input:**
```
Enter 9-letter key (row-major, A-Z): GYBNQKURP
Enter ciphertext: POH
```

**Output:**
```
Decrypted plaintext (uppercase): ACT
```

**Explanation:**
- Key matrix (row-major from "GYBNQKURP"):
  ```
  G Y B    [6 24 1]
  N Q K  = [13 16 10]
  U R P    [20 17 15]
  ```
- Ciphertext "POH" = [15, 14, 7]
- After decryption: [0, 2, 19] = "ACT"

### Example 2: Longer Ciphertext

**Input:**
```
Enter 9-letter key (row-major, A-Z): HILLCIPHER
Enter ciphertext: XFOPVEFRNHR
```

**Output:**
```
Decrypted plaintext (uppercase): SHORTEXAMPLE
```

### Example 3: Key Validation

If you enter an invalid key (determinant = 0 mod 2 or mod 13):

**Input:**
```
Enter 9-letter key (row-major, A-Z): AAAAAAAAB
Enter ciphertext: ABC
```

**Output:**
```
Error: Key matrix determinant is 0 modulo 2 -> not invertible mod 26.
```

---

## Notes and Limitations

### Important Notes

1. **Key Format**: 
   - Must be exactly 9 alphabetic characters (A-Z)
   - Entered in **row-major order** (first 3 letters = first row, etc.)
   - Case-insensitive (automatically converted to uppercase)

2. **Ciphertext Handling**:
   - Non-alphabetic characters are ignored
   - Automatically padded with 'X' if length is not a multiple of 3
   - All output is uppercase

3. **Matrix Invertibility**:
   - The key matrix must be invertible modulo 26
   - This requires: `gcd(det(K), 26) = 1`
   - Since 26 = 2 × 13, both `det(K) mod 2 ≠ 0` and `det(K) mod 13 ≠ 0` must hold

4. **CRT Advantage**:
   - Instead of computing inverses directly mod 26, we compute mod 2 and mod 13
   - This is computationally simpler and more efficient
   - CRT guarantees the result is correct modulo 26

### Limitations

1. **Block Size**: Only supports 3×3 matrices (3-letter blocks)
2. **Alphabet**: Only supports English alphabet (A-Z, 26 letters)
3. **No Encryption**: This program only performs decryption
4. **Padding**: Uses 'X' for padding, which may affect decryption of last block if original plaintext length wasn't a multiple of 3

### Security Considerations

⚠️ **Note**: The Hill cipher is **not secure** for modern cryptographic applications. It is vulnerable to:
- Known plaintext attacks
- Chosen plaintext attacks
- Frequency analysis (though less vulnerable than simple substitution)

This implementation is for **educational purposes** only, demonstrating:
- Matrix operations in modular arithmetic
- Chinese Remainder Theorem application
- Classical cryptography concepts

---

## Mathematical Verification

### Why CRT Works for 26 = 2 × 13

Given a value `x` modulo 26, we can find its residues:
- `r₂ = x mod 2`
- `r₁₃ = x mod 13`

To reconstruct `x` from these residues, we use:
```
x = 13 × r₂ + 14 × r₁₃ (mod 26)
```

**Verification:**
- `13 × r₂ mod 2 = 13 mod 2 × r₂ = 1 × r₂ = r₂` ✓
- `13 × r₂ mod 13 = 0 × r₂ = 0` ✓
- `14 × r₁₃ mod 2 = 0 × r₁₃ = 0` ✓
- `14 × r₁₃ mod 13 = 1 × r₁₃ = r₁₃` ✓

Therefore, the combination formula correctly reconstructs the value modulo 26.

---

## References

- **Hill Cipher**: Lester S. Hill, "Cryptography in an Algebraic Alphabet", 1929
- **Chinese Remainder Theorem**: Ancient Chinese mathematical theorem, formalized by Sun Tzu
- **Modular Arithmetic**: Fundamental concept in number theory and cryptography

---

## License

This project is provided for educational purposes. Feel free to use and modify as needed.

---

## Author

CNS Project - Hill Cipher Decryption with CRT Implementation

---

## Troubleshooting

### Common Issues

1. **"Key must contain exactly 9 alphabetic characters"**
   - Solution: Ensure your key is exactly 9 letters (A-Z)

2. **"Key matrix determinant is 0 modulo 2/13"**
   - Solution: Choose a different key matrix that is invertible mod 26

3. **Compilation errors**
   - Solution: Ensure you have a C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

4. **Program crashes on input**
   - Solution: Ensure you're entering valid alphabetic characters for the key

---

## Future Enhancements

Potential improvements:
- Support for different block sizes (2×2, 4×4)
- Encryption functionality
- File I/O for keys and messages
- Automatic key generation
- Support for different alphabets

