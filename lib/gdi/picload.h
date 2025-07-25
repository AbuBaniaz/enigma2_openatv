#ifndef __picload_h__
#define __picload_h__

#include <lib/base/ebase.h>
#include <lib/base/message.h>
#include <lib/base/thread.h>
#include <lib/gdi/gpixmap.h>
#include <lib/gdi/picexif.h>
#include <lib/python/python.h>

#ifndef SWIG
struct Cfilepara {
	char* file;
	unsigned char* pic_buffer;
	gRGB* palette;
	int palette_size;
	int bits;
	int id;
	int max_x;
	int max_y;
	int ox;
	int oy;
	std::string picinfo;
	bool callback;
	bool transparent;

	Cfilepara(const char* mfile, int mid, std::string size)
		: file(strdup(mfile)), pic_buffer(NULL), palette(NULL), palette_size(0), bits(24), id(mid), max_x(0), max_y(0),
		  ox(0), oy(0), picinfo(mfile), callback(true), transparent(true) {
		picinfo += "\n" + size + "\n";
	}

	~Cfilepara() {
		if (pic_buffer != NULL)
			delete[] pic_buffer;
		if (palette != NULL)
			delete[] palette;
		free(file);
	}

	void addExifInfo(std::string val) {
		picinfo += val + "\n";
	}
};
#endif

class ePicLoad : public eMainloop, public eThread, public sigc::trackable, public iObject {
	DECLARE_REF(ePicLoad);

	enum { F_PNG, F_JPEG, F_BMP, F_GIF, F_SVG, F_WEBP };

	void decodePic();
	void decodeThumb();
	void resizePic();

	Cfilepara* m_filepara;
	Cexif* m_exif;
	bool threadrunning;

	struct PConf {
		int max_x;
		int max_y;
		double aspect_ratio;
		uint32_t background;
		int resizetype;
		bool usecache;
		bool auto_orientation;
		int thumbnailsize;
		int test;
		PConf();
	} m_conf;

	struct Message {
		int type;
		enum { decode_Pic, decode_Thumb, decode_finished, decode_error, quit };
		Message(int type = 0) : type(type) {}
	};
	eFixedMessagePump<Message> msg_thread, msg_main;

	void gotMessage(const Message& message);
	void thread();
	int startThread(int what, const char* file, int x, int y, bool async = true);
	void thread_finished();
	bool getExif(const char* filename, int fileType = F_JPEG, int Thumb = 0);
	int getFileType(const char* file);

public:
	void waitFinished();
	PSignal1<void, const char*> PictureData;

	ePicLoad();
	~ePicLoad();

	RESULT startDecode(const char* filename, int x = 0, int y = 0, bool async = true);
	RESULT getThumbnail(const char* filename, int x = 0, int y = 0, bool async = true);
	RESULT setPara(PyObject* val);
	RESULT setPara(int width, int height, double aspectRatio, int as, bool useCache, int resizeType, const char* bg_str,
				   bool auto_orientation);
	PyObject* getInfo(const char* filename);
	SWIG_VOID(int) getData(ePtr<gPixmap>& SWIG_OUTPUT);
};

// for old plugins
SWIG_VOID(int)
loadPic(ePtr<gPixmap>& SWIG_OUTPUT, std::string filename, int x, int y, int aspect, int resize_mode = 0, int rotate = 0,
		unsigned int background = 0, std::string cachefile = "");

#endif // __picload_h__
