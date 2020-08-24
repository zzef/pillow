#ifndef USEFULUTILS_H
#define USEFULUTILS_H

int _split(const std::string& str, std::vector<std::string>& vec, char delim = ' '); 
bool is_empty(std::string line);
void init_mat(struct mtl* m);

#endif
