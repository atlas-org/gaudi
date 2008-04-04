extern "C"
#ifdef _WIN32
__declspec(dllimport)
#endif
int GaudiMain(int argc, char* argv[]);

int main(int argc, char* argv[] )   {
  return GaudiMain(argc, argv); 
}
         

