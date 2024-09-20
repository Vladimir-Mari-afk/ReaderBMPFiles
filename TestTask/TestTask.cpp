#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cstdint>

#pragma pack(push, 1)
struct BMPFileHeader
{
    uint16_t fileType{ 0x4D42 };
    uint32_t fileSize{ 0 };
    uint16_t reserved1{ 0 };
    uint16_t reserved2{ 0 };
    uint32_t offsetData{ 0 };
};

struct BMPInfoHeader
{
    uint32_t size{ 0 };
    int32_t width{ 0 };
    int32_t height{ 0 };
    uint16_t planes{ 1 };
    uint16_t bitCount{ 0 };
    uint32_t compression{ 0 };
    uint32_t imageSize{ 0 };
    int32_t xPixelsPerMeter{ 0 };
    int32_t yPixelsPerMeter{ 0 };
    uint32_t colorsUsed{ 0 };
    uint32_t colorsImportant{ 0 };
};
#pragma pack(pop)

class BMPReader
{
public:
    BMPReader() : data(nullptr) {}
    

    bool openBMP(const std::string& fileName)
    {
        std::ifstream inputFile(fileName, std::ios::binary);
        if (!inputFile.is_open())
        {
            std::cerr << "Error opening file" << std::endl;
            return false;
        }

        inputFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

        if (fileHeader.fileType != 0x4D42)
        {
            std::cerr << "Error: file is not BMP" << std::endl;
            return false;
        }

        inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

        std::cout << "Bit: " << infoHeader.bitCount << std::endl;

        if (infoHeader.bitCount != 24 && infoHeader.bitCount != 32)
        {
            std::cerr << "Unsupported BMP format: " << infoHeader.bitCount << " bit" << std::endl;
            return false;
        }

        inputFile.seekg(fileHeader.offsetData, inputFile.beg);

        //int rowSize = (infoHeader.bitCount / 8) * infoHeader.width;
        //data = new uint8_t[rowSize * infoHeader.height];
        //inputFile.read(reinterpret_cast<char*>(data), rowSize * infoHeader.height);

        size_t dataSize = (infoHeader.width * infoHeader.height * infoHeader.bitCount) / 8;
        data = std::make_unique<uint8_t[]>(dataSize);
		inputFile.read(reinterpret_cast<char*>(data.get()), dataSize);

        inputFile.close();
        return true;
    }

    void displayBmp() const
    {
        //int rowSize = (infoHeader.bitCount / 8) * infoHeader.width;
        if (!data)
        {
            std::cerr << "Error: no data to display" << std::endl;
            return;
        }

        const char whiteChar = ' ';
        const char blackChar = '#';

        for (int y = 0; y < infoHeader.height; ++y)
        {
            for (int x = 0; x < infoHeader.width; ++x)
            {
                int index = (x + y * infoHeader.width) * (infoHeader.bitCount / 8);
                uint8_t r = data[index + 2];
                uint8_t g = data[index + 1];
                uint8_t b = data[index + 0];
                
                if (r > 127 && g > 127 && b > 127)
                {
					std::cout << whiteChar;
                }
                else
                {
					std::cout << blackChar;
                }
            }
            std::cout << std::endl;
        }
    }

    void closeBmp()
    {
        data.reset();
    }

private:
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    std::unique_ptr<uint8_t[]> data;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <bmp_file_path>" << std::endl;
        return -1;
    }

    std::string filePath = argv[1];
    BMPReader bmpReader;

    if (!bmpReader.openBMP(filePath))
    {
        return -1;
    }

    bmpReader.displayBmp();
	bmpReader.closeBmp();

	return 0;
}

