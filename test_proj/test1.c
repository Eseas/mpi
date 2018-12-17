
#include <complex>
#include <iostream>
constexpr auto max_row = 22, max_column = 78, max_iteration = 20;
int main(){
    for(auto row = 0; row < max_row; ++row){
        for(auto column = 0; column < max_column; ++column){
            std::complex<float> z, c = {
                    (float)column * 2 / max_column - 1.5f,
                    (float)row * 2 / max_row - 1
            };
            int iteration = 0;
            while(abs(z) < 2 && ++iteration < max_iteration)
                z = pow(z, 2) + c;
            std::cout << (iteration == max_iteration ? '#' : '.');
        }
        std::cout << '\n';
    }
}