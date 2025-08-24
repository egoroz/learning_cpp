#include <iostream>

int toInt(const char* str){
	int result = 0;

	while(*str){
		result = result * 10 + (*str - '0');
		++str;
	}
	return result;
}

long long dfs(int depth, int k, int** arrs, int* sizes, int* indices, long long current_prod){
	if(depth == k){return current_prod;}
	
	long long sum = 0;
	for(int i = 0; i < sizes[depth]; ++i){
		bool flag = true;
		for(int j = 0; j < depth; ++j){
			if(indices[j] == i){flag = false; break;}
		}
		if(!flag){continue;}

		indices[depth] = i;
		current_prod *= arrs[depth][i];
		sum += dfs(depth+1, k, arrs, sizes, indices, current_prod);
	}
	return sum;
}

int main(int argc, char** argv){
	int k = argc - 1;
	int** arrs = new int*[k];
	int* sizes = new int[k];

	for(int i = 0; i < k; ++i){
		sizes[i] = toInt(argv[i + 1]);
		arrs[i] = new int[sizes[i]];
		for(int j = 0; j < sizes[i]; ++j){
			std::cin >> arrs[i][j];
		}
	}

	int* indices = new int[k];
	long long result;
	result = dfs(0, k, arrs, sizes, indices, 1);	

	std::cout << result;

	for(int i = 0; i < k; ++i){
		delete[] arrs[i];
	}

	delete[] indices;
	delete[] sizes;
	delete[] arrs;
}
