#ifndef POWERINFOMSG_H
#define POWERINFOMSG_H
struct PowerInfoMSG
{
  uint64_t magic;
  uint64_t timestamp;
  int32_t L[3];
 
  PowerInfoMSG() : L({1000, 1000, 1000}) {}
  bool isMagic() {return magic == 182345310123;}

  bool operator!=(const PowerInfoMSG& m)
  {
    return L[0] != L[0] || L[1] != L[1] || L[2] != L[2];
  }
};
#endif