#include <iostream>
#include <fstream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

MatrixXd readCSV(string file, int rows, int cols) {

  ifstream in(file);
  string line;

  int row = 0;
  int col = 0;
  MatrixXd res = MatrixXd(rows, cols);
  if (in.is_open()) {
    while (getline(in, line)) {
      char *ptr = (char *) line.c_str();
      int len = line.length();
      cout << "Line Length = " << line.c_str() << endl;

      col = 0;
      char *start = ptr;
      for (int i = 0; i < len; i++) {

        if (ptr[i] == ',') {
          res(row, col++) = atof(start);
          cout << "start = " << start << endl;

          start = ptr + i + 1;
        }
      }
      res(row, col) = atof(start);
      row++;
    }
    in.close();
  }
  return res;
}
