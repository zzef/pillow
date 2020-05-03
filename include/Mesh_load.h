#ifndef MESH_LOAD_H
#define MESH_LOAD_H

void split(const std::string& str, std::vector<std::string>& vec, char delim = ' '); 
void load_model(std::string path, std::string mtl_path, std::vector<Mesh*>& models);

#endif
