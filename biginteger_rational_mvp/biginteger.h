#pragma once

class BigInteger(){
private:

public:
    BigInteger();
    BigInteger(int64_t num);
    BigInteger(const std::string& str);
    BigInteger(const char* str, size_t sz);
    
};