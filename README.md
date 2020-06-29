C++ port of s0md3v's Arjun
Presently a "worked for me" initial commit with only minor variances in behavior from s0md3v's official version. It should compile and run on both Linux and Windows and arguably anything with a valid C++ compiler that supports C++11, which is basically everything. Requires OpenSSL for HTTPS otherwise its all self-contained.

s0md3v describes Arjun as a "HTTP Parameter Discovery Suite"; it basically performs basic hueristics on returned webpages and tries to brute force detect hidden/unknown parameters.
