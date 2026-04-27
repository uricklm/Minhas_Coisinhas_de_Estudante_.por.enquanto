#include <iostream>
#include <stdlib.h>
using namespace std;
int main () {
cout << "Bem-vindo ao jogo da Forca!" << endl;
    cout << "Digite a palavra secreta: ";
    string palavraSecreta;
    cin >> palavraSecreta;
    int tamanho = palavraSecreta.length();
    for (int i=0;i<tamanho;i++) {
        palavraSecreta[i] = tolower(palavraSecreta[i]);
    } 
    system("cls");
    cout << "A palavra secreta tem " << palavraSecreta.length() << " letras." << endl;
    int contador = 0;
    int tentativas = 0;
    while (true) {//ate ele adivinhar 
          cout << "Digite uma letra: ";
          char letra;
          cin >> letra;
          tentativas++;
          letra = tolower(letra);
          for (int i=0;i<tamanho;i++) {
              if (palavraSecreta[i] == letra) {
                  cout << "Parabéns! A letra " << letra << " está na palavra secreta na posicao " << i+1 << endl;
                  contador++;
              }
          }
          if (contador == tamanho) {
             cout << "Parabéns! Você adivinhou a palavra secreta " << palavraSecreta << " em " << tentativas  << endl;
             break;
          }
          }
    return 0;
}