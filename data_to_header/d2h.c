#include <stdio.h>
#include <string.h>
#include <io.h>
#include <stdbool.h>
#include <stdlib.h>

bool FileExists(const char* fileName)
{
	bool result = false;

	if (_access_s(fileName, 0) != -1) result = true;

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
	strcpy_s(fn, MAX_FILENAME_LENGTH, fileName);

	int size = (int)strnlen_s(fn, MAX_FILENAME_LENGTH);   // Get size in bytes

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
	fopen_s(&file,fileName, "rb");

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



int main(int argc, char** argv)
{
	char extension[6] = "     ";

	if (argc < 3)
	{
		printf("Usage: \".\\data_to_header <in_file> <out_file> [extension]\"\n\n");
		return -1;
	}
	if (argc == 4)
	{
		if (strnlen_s(argv[3],6) > 5)
		{
			printf("The file extension %s is too long... \n\n",argv[3]);
			return -1;
		}
		else {
			strcpy_s(extension, 6, argv[3]);
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
			if (strnlen_s(r_value, 6) > 5)
			{
				printf("The file extension %s is too long... \n\n", r_value);
				return -1;
			}
			else
			{
				strcpy_s(extension, 6, r_value);
				printf("File extension set: %s\n", extension);
			}
		}
	}

	if (!FileExists(argv[1]))
	{
		printf("File \"%s\" does not exist\n", argv[1]);
		return -1;
	}
	
	if (strncmp(extension, ".png", 6) || strncmp(extension, ".bmp", 6) || strncmp(extension, ".tga", 6)
	|| strncmp(extension, ".jpg", 6) || strncmp(extension, ".gif", 6) || strncmp(extension, ".pic", 6)
	|| strncmp(extension, ".psd", 6) || strncmp(extension, ".ttf", 6) || strncmp(extension, ".otf", 6)
	|| strncmp(extension, ".wav", 6) || strncmp(extension, ".ogg", 6) || strncmp(extension, ".flac", 6)
	|| strncmp(extension, ".mp3", 6) || strncmp(extension, ".otf", 6) || strncmp(extension, ".xm", 6)
	|| strncmp(extension, ".mod", 6))
	{
		FILE* outFile;
		errno_t err;
		err = fopen_s(&outFile, argv[2], "w+");
		if (err == 0)
		{
			printf("Opened %s succesfully\n", argv[2]);
		}
		else
		{
			printf("Could not open %s succesfully\n", argv[2]);
			return -1;
		}

		unsigned int size=0;
		unsigned char* data = LoadFileData(argv[1], &size);
		if (data == NULL)
		{
			return -1;
		}

		const char* arrayName = GetFileNameWithoutExt(argv[2]);

		fprintf_s(outFile, "const char* %s_ext = \"%s\";\n\n",arrayName,extension);

		fprintf_s(outFile, "int %s_size=%d;\n\n", arrayName, size);

		fprintf_s(outFile, "unsigned char %s[] = {",arrayName);

		unsigned int n,count=0;
		for (n = 0; n < size-1; n++)
		{
			fprintf_s(outFile, "%d,", data[n]);
			count++;
			if (count == 20)
			{
				fprintf_s(outFile, "\n");
				count = 0;
			}
		}
		fprintf_s(outFile, "%d};", data[size-1]);
		free(data);
	}

	_fcloseall();
	return 0;
}