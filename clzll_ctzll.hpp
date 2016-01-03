#ifdef _WIN32
    unsigned long long static const BOT32MASK = (1LL << 32) - 1LL;
    unsigned long long static const TOP32MASK = (-1LL) & ~BOT32MASK;
    int clzll(unsigned long long input) {
        if((TOP32MASK & input) != 0) {
            return __builtin_clzl((ulong)(input >> 32));
        } else {
            return __builtin_clzl((ulong)(input & TOP32MASK));
        } 
    }
    int ctzll(unsigned long long input) {
        if((BOT32MASK & input) != 0) {
            return __builtin_ctzl((ulong)input);
        } else {
            return 32 + __builtin_ctzl((ulong)(input >> 32));
        }
    }
#elif __linux__
    int clzll(unsigned long long input) {
        return __builtin_clzl(input);
    }
    int ctzll(unsigned long long input) {
        return __builtin_ctzl(input);
    }
#else
    printf("OS not supporeted!\n");
    exit(-1);
#endif
