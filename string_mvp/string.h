#include <iostream>
#include <cstring>
//#include <algorithm> // for std::fill and std::copy

class String{
private:
	char* str;
	size_t sz;
	size_t cap;
	static constexpr size_t DEFAULT_CAPACITY = 8;
	static constexpr size_t CAPACITY_EXPANSION = 2;

public:
	explicit String(const char* other) {
		if(!other){cap = 1; sz = 0; str = new char[cap]; str[sz] = '\0'; return;}
		sz = strlen(other);
		cap = sz + 1;
		str = new char[cap];
		memcpy(str, other, sz + 1);
	}

	String() {
		cap = 1;
		sz = 0;
		str = new char[cap];
		str[sz] = '\0';
	}


	String(size_t n, char c): str(new char[n + 1]), sz(n), cap(n + 1){
		memset(str, c, n);
		str[n] = '\0';
		if(c == '\0'){sz = 0;}
	}

	String(const String& other): str(new char[other.cap]), sz(other.sz), cap(other.cap){
		memcpy(str, other.str, sz + 1);
	} 

	~String(){
		delete[] str;
	}

	void swap(String& other){
		std::swap(str, other.str);
		std::swap(sz, other.sz);
		std::swap(cap, other.cap);
	}

	// Copy and swap idiom
	String& operator=(String other) & { // only lvalue
		swap(other);
		return *this;
	}

	// operators without implicit conversion
	bool operator<(const String& other) const{
		return strcmp(str, other.str) < 0;
	}

	bool operator>(const String& other) const{
		return other < *this;
	}

	bool operator<=(const String& other) const{
		return !(other < *this);
	}

	bool operator>=(const String& other) const{
		return !(*this < other);
	}

	bool operator==(const String& other) const{
		return strcmp(str, other.str) == 0;
	}

	bool operator!=(const String& other) const{
		return strcmp(str, other.str) != 0;
	}
	

	char& operator[](size_t n) {
		return str[n];
	}

	const char& operator[](size_t n) const{
		return str[n];
	}
	
	size_t length() const{
		return sz;
	}

	size_t size() const{
		return sz;
	}

	size_t capacity() const{
		return cap;
	}

	void push_back(char c){
		if (cap == 0){
			cap = DEFAULT_CAPACITY;
			str = new char[cap];
		}
		if(sz + 1 == cap){
			char* new_str = new char[cap *= CAPACITY_EXPANSION];
			memcpy(new_str, str, sz + 1);
			delete[] str;
			str = new_str;
		}
		str[sz++] = c;
		str[sz] = '\0';
	}

	void pop_back(){
		str[--sz] = '\0';
	}

	char& front() {
		return str[0];
	}

	const char& front() const {
		return str[0];
	}

	char& back() {
		return str[sz-1];
	}

	const char& back() const{
		return str[sz-1];
	}

	String& operator+=(char c){
		this->push_back(c);
		return *this;
	}

	String& operator+=(const String& other){
		if (cap == 0){
			cap = (DEFAULT_CAPACITY < other.sz + 1) ? other.sz + 1 : DEFAULT_CAPACITY;
			str = new char[cap];
		}
		if(sz + other.sz + 1 > cap){
			cap = (cap * CAPACITY_EXPANSION < sz + other.sz + 1) ? sz + other.sz + 1 : cap * CAPACITY_EXPANSION;
			char* new_str = new char[cap];
			memcpy(new_str, str, sz);
			delete[] str;
			str = new_str;
		}
		memcpy(str + sz, other.str, other.sz + 1);	
		return *this;
	}

	explicit String(char c): str(new char[2]), sz(1), cap(2){
		str[0] = c;
		str[1] = '\0';
		if(c == '\0'){sz = 0;}
	}

	explicit String(size_t n): str(new char[n + 1]), sz(0), cap(n+1){
		str[0] = '\0';
	}

	explicit operator const char*() const{
		return str;
	}
	
	size_t find(const String& substring) const{
		const char* pos;
		return (pos = strstr(str, static_cast<const char*>(substring))) ? static_cast<size_t>(pos - str) : sz;
	}

	size_t rfind(const String& substring) const{
		const char* main_string = str + sz - substring.size();
		if(str > main_string){return sz;}
		while(strstr(main_string, static_cast<const char*>(substring)) == nullptr){
			if(str >= main_string--){return sz;}
		}
		return static_cast<size_t>(main_string - str);
	}

	String substr(size_t start, size_t count) const{
		String substring(count);
		memcpy(substring.str, str + start, count);
		substring.sz = count;
		substring[count] = '\0';
		return substring;
	}

	bool empty() const{
		return sz == 0;
	}

	void clear(){
		if(!str){return;}
		str[0] = '\0';
		sz = 0;
	}

	void shrink_to_fit(){
		if(sz + 1 == cap){return;}
		cap = sz + 1;
		char* new_str = new char[sz + 1];
		memcpy(new_str, str, sz + 1);
		delete[] str;
		str = new_str;
	}

	char* data(){
		return str;
	}

	const char* data() const{
		return str;
	}
};


String operator""_str(const char* str, size_t){
	return String(str);
}

// useful compiler optimization
String operator+(const String& a, const String& b){
	String copy = a;
	copy += b;
	return copy;
}

std::ostream& operator<<(std::ostream& os, const String& string){
	if(!string.data()){return os;}
	return os << string.data();
}

std::istream& operator>>(std::istream& is, String& string){
	string.clear();
	is >> std::ws;
	while(is.good()){
		int next_char = is.peek();
		if(next_char == EOF || std::isspace(static_cast<unsigned char>(next_char))){
			break;
		}
		char c;
		is.get(c);
		string.push_back(c);
	}
	return is;
}