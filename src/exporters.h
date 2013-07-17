#ifndef __KARSTGEN_EXPORTERS_H
#define __KARSTGEN_EXPORTERS_H

#include <vector>
#include <string>
#include <memory>

void export_avr(std::vector<MCMesh> meshes, std::string fileName);
void export_wavefront_obj(std::vector<MCMesh> meshes, std::string fileName);

#endif //__KARSTGEN_EXPORTERS_H
