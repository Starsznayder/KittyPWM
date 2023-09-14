#ifndef CUSTOMSTRING_H
#define CUSTOMSTRING_H

#define MAX_STRING_LENGTH 200

class CustomString
{
public:

CustomString() : index_(0) 
{
  buffer_ = (char*)malloc(MAX_STRING_LENGTH);
}

~CustomString()
{
  free(buffer_);
}

char push_back(const char& t)
{
  buffer_[index_] = t;
  //Serial.print(buffer_[index_]);
	incIndex();
  return t;
}

void clear()
{
	index_ = 0;
}

void tokenize()
{
  for (int i = 0; i < index_; ++i)
  {
	  if (buffer_[i] == '&')
		{
			buffer_[i] = '\0';
		}
  }
}

int32_t parseInt(const char* prefix, uint8_t prefixSize)
{
  int16_t pos1 = indexOf(prefix, prefixSize) + prefixSize;

  if (pos1 > 0)
  {
    return atol(&buffer_[pos1]);
  }

  return -1;
}

bool strCmp(const char* ref, const uint16_t refSize)
{
	if(index_ < refSize)
  {
		return false;
  }

  for (uint16_t i = 0; i < refSize; ++i)
  {
	  if (buffer_[i] != ref[i])
		{
			return false;
		}
  }
  return true;
}

int16_t indexOf(const char* str, uint16_t size, uint16_t begIdx = 0)
{
	if(size < index_)
  {
		for (uint16_t i = begIdx; i + size <= index_; ++i)
		{
			uint16_t ss = 0;
			for (uint16_t j = 0; j < size; ++j)
			{
				ss += str[j] == buffer_[j + i];
			}

			if (ss == size)
			{
				return i;
			}
		}
	}
	return -1;
}

uint16_t size() {return index_;}

private:
	char* buffer_;
	uint16_t index_;
  CustomString(const CustomString&) = delete;
  CustomString & operator=(const CustomString&) = delete;
	
	void incIndex()
	{
		++index_;
		if (index_ >= MAX_STRING_LENGTH)
		{
			index_ = 0;
		}
	}
};

#endif