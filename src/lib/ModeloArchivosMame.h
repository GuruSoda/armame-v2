#ifndef _MODELOARCHIVOSMAME_H_
#define _MODELOARCHIVOSMAME_H_

#include <boost/algorithm/string/case_conv.hpp>
#include <string>
#include <map>

#include <GuruDirectorio.hpp>
#include <GuruMameListInfo.h>
#include <GuruUnzip.h>
#include <GuruSHA1.h>
#include <GuruCRC.h>

using namespace boost::algorithm;
using namespace std;

class ModeloArchivosMame {
public:
	ModeloArchivosMame() {};
	virtual ~ModeloArchivosMame() {};

	void cargar_directorio(const string& directorio, map<string, vector<rom> >& mapa_directorio);
	vector<rom>& contenido_zip(const string& archivozip, vector<rom>& contenido);

	class iterator;
	friend class iterator;

	iterator begin(const string& directorio) { return iterator(*this, directorio);};
	iterator end() {return iterator(*this, true);};

	class iterator	{
	public:
		iterator (ModeloArchivosMame& Archivos, const string& directorio):_archivos(Archivos),_directorio(directorio),it(directorio) {
				if (!it->sacar_nombre().empty())
					_archivos.contenido_zip(_directorio + it->sacar_nombre(), archivo_actual);
		};
		iterator (ModeloArchivosMame& Archivos, bool):_archivos(Archivos) {};

		bool operator!=(const iterator& it2) const {
			if (this->it != it2.it) return true;
			else return false;
		}

		// paso al siguiente archivo y si no puede cargar la informacion del zip
		// pasa al siguiente y trata de cargar la informacion.
		void operator++() {
			while(8){
				++it;

				if (it == itfinal) break;

				if (it->EsArchivo()){
					_archivos.contenido_zip(_directorio + it->sacar_nombre(), archivo_actual);

					if (archivo_actual.empty()) continue;
					else break;					
				}
			}
		}

		string nombre () const {
			return it->sacar_nombre();
		}

		vector<rom>& contenido () {
			return archivo_actual;
		}

	protected:
		ModeloArchivosMame& _archivos;
		string 	_directorio;
		vector<rom> archivo_actual;
		GuruDirectorio::iterator_recursivo it;
		GuruDirectorio::iterator_recursivo itfinal;
	};
};

#endif /* _MODELOARCHIVOSMAME_H_ */
