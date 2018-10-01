#pragma once
enum class DataType
{
	bmp,
	jpg,
};

class ImageData
{
private:
	DataType _dataType;
	unsigned int _width;
	unsigned int _height;
	//std::vector<char> _data;
public:
	ImageData();
	~ImageData();
	unsigned int GetWidth();
	unsigned int GetHeight();
	char* GetBufferPointer();

};

