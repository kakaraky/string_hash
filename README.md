# string_hash
Fast string hashing designed to provide a relatively good hash distribution over the lower part of the hash, making it a good candidate for hash tables with power of 2 sizes. It uses parallelism by working on 4 bytes at one time, with a minimal amount of simple operations widely available in most CPUs.

It can be used to reduce the string comparison length by 4, since the hash value itself provides 4 bytes of data recovery.

Note: strings here are in fact any array of data, not limited to just alphanumeric arrays as the hash function works well with any type of data and reasonable explanations for constants used are provided, as opposed to other algorithms that use 'magic' numbers.

# Requirements
1. Hash density resides in the lower part of the hash. While the hash function does not provide a good avalanche property, this requirement is about the minimum operations needed to provide good modulo 2**n distribution for hash tables, given n < 32, and at the same time to maintain good hash distribution in subsequent iterations.

2. On strings up to the hash size the hash function must bijective. Hashes with this property can be used as recovery information. In other words, the hash can be used to recover any sequence of 'missing' bytes up to the hash size. However we are not after data recovery here, but after a consequence of the above: once the hash is matched one only needs to compare the string length - hash size to match the whole string. So for strings with length > 4 bytes, one needs to compare only (length - 4) bytes, while for strings of lenght <= 4, there is no need to compare the strings at all!

3. Strings are aligned on hash size boundary. This happens implicitly most of the time, the reason why it's mentioned is because the hash function may touch up to 3 bytes outside the string and this requirement will prevent it from crossing pages.
