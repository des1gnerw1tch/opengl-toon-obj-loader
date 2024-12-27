#include "PPM.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <Pixel.hpp>

// Constructor loads a filename with the .ppm extension
PPM::PPM(std::string fileName){
  std::ifstream inputFile;
  inputFile.open(fileName);
  if (inputFile.is_open())  {
    bool foundP3 = false;
    bool foundDimensions = false;
    bool foundRange = false;
    bool readingPixels = false;
    std::string line;
    while (std::getline(inputFile,line))  {
      std::stringstream stream(line);
      std::string chunk_of_data;
      while (stream >> chunk_of_data)  {
        if (chunk_of_data[0] == '#')  {
          break;
        }
        else if (false==foundP3)  {
        if (chunk_of_data=="P3")  {
            foundP3 = true; 
        }
        } else if (false==foundDimensions)  {
          m_width = std::stoi(chunk_of_data);
          stream >> chunk_of_data;
          m_height = std::stoi(chunk_of_data);
          foundDimensions = true;  
        } else if (false==foundRange)  {
          m_maxRange = std::stoi(chunk_of_data);
          foundRange = true;
        } else {
          Pixel p;
          p.r = std::stoi(chunk_of_data);
          p.g = getNextInteger(inputFile, stream);
          p.b = getNextInteger(inputFile, stream);
          m_PixelData.push_back(p);
        }
      }
    }
  inputFile.close();
  } else {
    std::cout << "Could not open file" << std::endl;
  }
}

u_int8_t PPM::getNextInteger(std::ifstream& inputFile, std::stringstream& stream) {
    std::string line;
    std::string chunk_of_data;
    
    if (stream >> chunk_of_data && chunk_of_data[0] != '#')    {
        u_int8_t result = std::stoi(chunk_of_data);
        return result;
    } else {
        std::getline(inputFile, line);
        stream = std::stringstream(line);
        return getNextInteger(inputFile, stream);
    }
}

// Destructor deletes(delete or delete[]) any memory that has been allocated
// or otherwise calls any 'shutdown' or 'destroy' routines for this deletion
// to occur.
PPM::~PPM(){
    // No memory to reclaim, std::vector cleans up itself
}

// Saves a PPM Image to a new file.
void PPM::savePPM(std::string outputFileName) const {
  std::ofstream newFile;
  newFile.open(outputFileName);
  newFile << "P3" << std::endl;
  newFile << "# saved by us" << std::endl;
  newFile << m_width << " " << m_height << std::endl;
  newFile << m_maxRange << std::endl;
  for (auto p : m_PixelData)  {
    newFile << (int)p.r << std::endl;
    newFile << (int)p.g << std::endl;
    newFile << (int)p.b << std::endl;
  }
  newFile.close();
}

// Darken halves (integer division by 2) each of the red, green
// and blue color components of all of the pixels
// in the PPM. Note that no values may be less than
// 0 in a ppm.
void PPM::darken(){
   for (auto& p : m_PixelData)  {
      p.r = std::clamp(((int)p.r) / 2, 0, m_maxRange);
      p.g = std::clamp(((int)p.g) / 2, 0, m_maxRange);
      p.b = std::clamp(((int)p.g) / 2, 0, m_maxRange);
   }
}

// Lighten doubles (integer multiply by 2) each of the red, green
// and blue color components of all of the pixels
// in the PPM. Note that no values may be greater than
// 255 in a ppm.
void PPM::lighten(){
   for (auto& p : m_PixelData)  {
      p.r = std::clamp(((int)p.r) * 2, 0, m_maxRange);
      p.g = std::clamp(((int)p.g) * 2, 0, m_maxRange);
      p.b = std::clamp(((int)p.g) * 2, 0, m_maxRange);
   }
}

// Sets a pixel to a specific R,G,B value 
// Note: You do not *have* to use setPixel in your implementation, but
//       it may be useful to implement.
void PPM::setPixel(int x, int y, uint8_t R, uint8_t G, uint8_t B){
    // TODO: Optional to implement.
}

void PPM::VerticalFlip() {
  std::vector<Pixel> result;
  std::vector<std::vector<Pixel>> rows;
  
  for (int row = 0; row < m_height; row++) {
    rows.push_back(std::vector<Pixel>());
    for (int col = 0; col < m_width; col++)  { 
      rows[row].push_back(m_PixelData[row * m_width + col]);
    }
  }

  for (int i = rows.size() - 1; i >= 0; i--)  {
    for (Pixel p : rows[i]) {
      result.push_back(p);
    }
  }
  m_PixelData = result;
}
