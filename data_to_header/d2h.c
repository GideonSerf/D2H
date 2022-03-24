#ifdef _MSC_VER
#define VC
#elif __GNUC__
#define GCC
#endif

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
	#define ACCESS(p,m) _access_s(p, m)
#endif // _WIN32
#ifdef __linux__
	#include <unistd.h>
#define ACCESS(p,m) access(p,m)
#endif // __linux__

#include <stdbool.h>
#include <stdlib.h>

#ifdef VC
#define STRNLEN(String, MaxSize) strnlen_s(String,MaxSize)
#define FPRINTF fprintf_s
#define FOPEN(F,fname,AS) fopen_s(&F,fname,AS)
#define STRNCPY(dest,src,n) strcpy_s(dest,n,src)
#endif // VC

#ifdef GCC
#define STRNLEN(String, MaxSize) strnlen(String,MaxSize)
#define FPRINTF fprintf
#define FOPEN(F,fname,AS) F=fopen(fname,AS);
#define STRNCPY(dest,src,n) strncpy(dest,src,n)
#endif // GCC

//Functions from raylib
bool FileExists(const char* fileName)
{
	bool result = false;

	if (ACCESS(fileName, 0) != -1) result = true;

	return result;
}

const char* GetFileExtension(const char* fileName)
{
	const char* dot = strrchr(fileName, '.');

	if (!dot || dot == fileName) return NULL;

	return dot;
}

const char* GetFileNameWithoutExt(const char* fileName)
{
#define MAX_FILENAME_LENGTH   128
	static char fn[MAX_FILENAME_LENGTH];
	STRNCPY(fn, fileName, MAX_FILENAME_LENGTH);

	int size = (int)STRNLEN(fn, MAX_FILENAME_LENGTH);   // Get size in bytes

	for (int i = 0; (i < size) && (i < MAX_FILENAME_LENGTH); i++)
	{
		if (fn[i] == '.')
		{
			// NOTE: We break on first '.' found
			fn[i] = '\0';
			break;
		}
	}

	return fn;
}

unsigned char* LoadFileData(const char* fileName, int* bytesRead)
{
	unsigned char* data = NULL;
	*bytesRead = 0;

	FILE* file;
	FOPEN(file,fileName, "rb");

	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		fseek(file, 0, SEEK_SET);

		if (size > 0)
		{
			data = (unsigned char*)malloc(sizeof(unsigned char) * size);

			if (data)
			{
				int count = (int)fread(data, sizeof(unsigned char), size, file);

				*bytesRead = count;

				if (count != size) printf("[%s] File partially read", fileName);
			}
		}
		else printf("[%s] File could not be read", fileName);

		fclose(file);
	}
	else printf("[%s] File could not be opened", fileName);

	return data;
}

const char* GetFileName(const char* filePath)
{
	const char* fPathCpy = filePath;
	const char* fileName = NULL;
	if (filePath != NULL)
	{
		for (; filePath = strpbrk(filePath, "\\/"), filePath != NULL; fileName = filePath++) {}
	}

	if (!fileName) return fPathCpy;

	return fileName + 1;
}
//=================================================================================================


int main(int argc, char** argv)
{
	#define MAX_EXTENSION_LENGTH 5

	char extension[MAX_EXTENSION_LENGTH+1]={0};

	if (argc < 3)
	{
		printf("Usage: \".\\data_to_header <in_file> <out_file> [extension]\"\n\n");
		return -1;
	}
	if (argc == 4)
	{
		if (STRNLEN(argv[3], MAX_EXTENSION_LENGTH+1) > MAX_EXTENSION_LENGTH)
		{
			printf("The file extension %s is too long... \n\n",argv[3]);
			return -1;
		}
		else {
			STRNCPY(extension, argv[3], MAX_EXTENSION_LENGTH+1);
			printf("File extension set: %s\n", extension);
		}
	}
	else if (argc == 3)
	{
		const char* r_value = GetFileExtension(argv[1]);
		if (!r_value)
		{
			printf("Could not find extension of file %s\n", argv[1]);
			return -1;
		}
		else
		{
			if (STRNLEN(r_value, MAX_EXTENSION_LENGTH + 1) > MAX_EXTENSION_LENGTH)
			{
				printf("The file extension %s is too long... \n\n", r_value);
				return -1;
			}
			else
			{
				STRNCPY(extension, r_value, MAX_EXTENSION_LENGTH + 1);
				printf("File extension set: %s\n", extension);
			}
		}
	}

	if (!FileExists(argv[1]))
	{
		printf("File \"%s\" does not exist\n", argv[1]);
		return -1;
	}
	
	if (strncmp(extension, ".png", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".bmp", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".tga", MAX_EXTENSION_LENGTH + 1)
	|| strncmp(extension, ".jpg", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".gif", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".pic", MAX_EXTENSION_LENGTH + 1)
	|| strncmp(extension, ".psd", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".ttf", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".otf", MAX_EXTENSION_LENGTH + 1)
	|| strncmp(extension, ".wav", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".ogg", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".flac", MAX_EXTENSION_LENGTH + 1)
	|| strncmp(extension, ".mp3", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".otf", MAX_EXTENSION_LENGTH + 1) || strncmp(extension, ".xm", MAX_EXTENSION_LENGTH + 1)
	|| strncmp(extension, ".mod", MAX_EXTENSION_LENGTH + 1))
	{
		FILE* outFile;
#ifdef VC

		errno_t err;
		err = FOPEN(outFile, argv[2], "w+");
		if (err == 0)
		{
			printf("Opened %s succesfully\n", argv[2]);
		}
		else
		{
			printf("Could not open %s succesfully\n", argv[2]);
			return -1;
		}
#endif
#ifdef GCC
		FOPEN(outFile,argv[2], "w+");
		if (outFile != NULL)
		{
			printf("Opened %s succesfully\n", argv[2]);
		}
		else
		{
			printf("Could not open %s succesfully\n", argv[2]);
			return -1;
		}
#endif


		unsigned int size=0;
		unsigned char* data = LoadFileData(argv[1], &size);
		if (data == NULL)
		{
			return -1;
		}
		
		const char* arrayName = GetFileNameWithoutExt(GetFileName(argv[2]));

		FPRINTF(outFile, "const char* %s_ext = \"%s\";\n\n",arrayName,extension);

		FPRINTF(outFile, "int %s_size=%d;\n\n", arrayName, size);

		FPRINTF(outFile, "unsigned char %s[] = {",arrayName);

		unsigned int n,count=0;
		for (n = 0; n < size-1; n++)
		{
			FPRINTF(outFile, "%d,", data[n]);
			count++;
			if (count == 20)
			{
				FPRINTF(outFile, "\n");
				count = 0;
			}
		}
		FPRINTF(outFile, "%d};", data[size-1]);
		free(data);
		fclose(outFile);
	}

	
	return 0;
}
