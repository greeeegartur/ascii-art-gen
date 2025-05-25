//
// Projekt kasutab stb_image.h library, et sisendid muuta ASCII kunstiks.
// Selles .cpp failis on programmi loogika selle alusel.
//
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize2.h"
#include <filesystem>
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

// Näitab töö kestvust (täiustav UI element)
void showProgress(int current, int total) {
    const int barWidth = 40;
    float progress = (float)current / total;
    cout << "\t[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << setw(3) << int(progress * 100.0) << "%\r";
    cout.flush();
}

void title() {
    cout << "\n\n\t\t╔══════════════════════════════════════╗\n";
    cout << "\t\t║            ASCII GEN v1.0            ║\n";
    cout << "\t\t║                                      ║\n";
    cout << "\t\t║   Mattias Antsov, Gregor Artur Mäe   ║\n";
    cout << "\t\t╚══════════════════════════════════════╝\n\n";
}

int main() {

    // stbi_load jaoks vajalikud parameetrid
    int width, height, channels;

    // Võtab kasutaja failisisendi
    string in_filename, out_filename;
    title();
    cout << "\tPalun sisesta failinimi (koos failimuutujaga): ";
    cin >> in_filename;

    // Kontrollib kas faili on olemas
    ifstream file;
    file.open(in_filename);
    if (!file) {
        cerr << "\tFaili " << in_filename << " ei leitud. :/" << endl;
        return 1;
    }
    file.close();

    // Laeb pikslite andmed ühte muutujasse
    unsigned char* data = stbi_load(in_filename.c_str(), &width, &height, &channels, 3);
    if (!data) {
        cerr << "\tVigane sisend. Kas fail on pilt?" << endl;
        return 1;
    }

    // Dünaamiline laius ja kõrgus ning selle kuvamine
    float aspect_ratio = 0.75f;
    int auto_width = max(min(width / 4, 240), 60);
    int auto_height = static_cast<int>((height * auto_width * aspect_ratio) / width);
    cout << "\n\tArvutatud mõõtmed: " << auto_width << " x " << auto_height << " (laius x kõrgus) – sobib enamikes tekstivaatajates" << endl;

    // Laiuse/kõrguse määramise küsimine
    string choice;
    int out_width = auto_width, out_height = auto_height;
    cout << "\tKas soovid määrata laiuse ja kõrguse ise? [Y/N]: ";
    cin >> choice;
    if (choice == "y" || choice == "Y") {
        cout << "\n\tSisesta soovitud laius (nt 480): ";
        cin >> out_width;
        cout << "\tSisesta soovitud kõrgus (nt 360): ";
        cin >> out_height;
        if (out_width <= 0 || out_height <= 0) {
            cerr << "\tVigased mõõdud. Programm katkestub.\n";
            stbi_image_free(data);
            return 1;
        }
    }
    // Puhastab puhvri
    cin.ignore();

    // Output faili nime küsimine, jätab samaks mis sisend kui jäetakse vahele
    cout << "\n\tSisesta väljundfaili nimi: ";
    cin >> out_filename;

    // Lisab failile .txt failimuutuja
    if (out_filename.empty()) {
        size_t dotPos = in_filename.find_last_of('.');
        out_filename = (dotPos != string::npos ? in_filename.substr(0, dotPos) : in_filename) + ".txt";
    } else if (out_filename.find(".txt") == string::npos) {
        out_filename += ".txt";
    }

    // Kuna ASCII sümbolid pole ruudud (nagu on piksel), tuleb pikslite andmed muuta väiksemaks stb resize meetodiga,
    // vastasel juhul vaikeparameetriga ilmuksid sümbolid vertikaalselt teineteise peal ning ASCII kunsti poleks näha
    unsigned char* resized = new unsigned char[out_width * out_height * 3];
    stbir_resize_uint8_srgb(data, width, height, 0, resized, out_width, out_height, 0, STBIR_RGB);
    stbi_image_free(data);

    // Pikslid -> ASCII sümboliteks loogika
    ofstream out(out_filename);
    int last_percent = -1;
    for (int y = 0; y < out_height; y += 5) {
        for (int x = 0; x < out_width; x += 2) {
            int i = (y * out_width + x) * 3;
            char c = pixelToChar(resized[i], resized[i + 1], resized[i + 2]);
            out << c;
        }
        out << endl;
        int current_percent = (100 * (y + 1)) / out_height;
        if (current_percent != last_percent) {
            showProgress(y + 1, out_height);
            last_percent = current_percent;
        }
    }
    cout << "\t[========================================] 100%\n";
    cout << "\n\tProgramm on töö edukalt lõpetanud! Tulemus on nähtav root kaustas." << endl;

    // Vabastab andmed pärast teisendust
    out.close();
    delete[] resized;
    return 0;
}
