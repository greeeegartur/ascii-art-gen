//
// Projekt kasutab stb_image.h library, et sisendid muuta ASCII kunstiks.
// Selles .cpp failis on programmi loogika selle alusel.
//
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <filesystem>

#include "stb_image_resize2.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Kasutatavad ASCII sümbolid, saab täiendada
const char* ascii_chars = "@%#*+=-:. ";

// Meetod, mis muudab piksli grayscale värvi, otsustab selle järgi indeksi ascii_chars muutujast
// ja tagastab selle piksli valitud ASCII sümboli.
char pixelToChar(unsigned char r, unsigned char g, unsigned char b) {
    float gray = 0.299f*r + 0.587f*g + 0.114f*b;
    int index = (gray / 255.0f) * (strlen(ascii_chars) - 1);
    return ascii_chars[index];
}

int main() {

    // stbi_load jaoks vajalikud parameetrid
    int width, height, channels, out_width;

    // Võtab kasutaja failisisendi
    string in, out_filename;
    cout << "Palun sisesta failinimi koos failimuutujaga): ";
    cin >> in;

    // Check if file exists
    if (!filesystem::exists(in)) {
        cerr << "Faili ei leitud: " << in << endl;
        return 1;
    }

    // Laeb pikslite andmed ühte muutujasse
    unsigned char* data = stbi_load(in.c_str(), &width, &height, &channels, 3);

    if (!data) {
        cerr << "Vigane sisend." << endl;
        return 1;
    }

    //laiuse küsimine
    cout << "Sisesta soovitud laiuse väärtus : ";
    cin >> out_width;
    if (out_width <= 0) {
        out_width = 540;
        cout << "Vale sisestus. Kasutan vaikeväärtust: " << out_width << "\n";
    }

    int out_height = (height * out_width) / width / 2; // kõrgus on siin dünaamiline vastavalt pildile

    // Kuna ASCII sümbolid pole ruudud (nagu on piksel), tuleb pikslite andmed muuta väiksemaks stb resize meetodiga,
    // vastasel juhul vaikeparameetriga ilmuksid sümbolid vertikaalselt teineteise peal ning ASCII kunsti poleks näha
    unsigned char* resized = new unsigned char[out_width * out_height * 3];
    stbir_resize_uint8_srgb(data, width, height, 0, resized, out_width, out_height, 0, STBIR_RGB);
    stbi_image_free(data);

    // soovitud output faili nime küsimine
    cout << "Soovid määrata väljundfaili nime? (vajuta Enter vahelejätmiseks): ";
    cin.ignore();
    getline(cin, out_filename);

    if (out_filename.empty()) {
        size_t dotPos = in.find_last_of('.');
        out_filename = (dotPos != string::npos ? in.substr(0, dotPos) : in) + ".txt";
    } else if (out_filename.find(".txt") == string::npos) {
        out_filename += ".txt";
    }

    ofstream out(out_filename);
    for (int y = 0; y < out_height; y += 5) {
        for (int x = 0; x < out_width; x += 2) {
            int i = (y * out_width + x) * 3;
            char c = pixelToChar(resized[i], resized[i + 1], resized[i + 2]);
            out << c;
        }
        out << endl;
    }
    cout << "Programm on töö edukalt lõpetanud! Tulemus on nähtav root kaustas." << endl;

    // Vabastab andmed
    out.close();
    delete[] resized;
    return 0;
}
