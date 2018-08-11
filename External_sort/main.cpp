//
//  main.cpp
//  External_sort
//
//  Created by Vu Tuan Dat on 8/11/18.
//

#include <iostream>
#include <vector>

// Queue node to be put in priority queue
struct QueueNode {
    // line stored
    char* element;
    // index of file
    int index;
};

// comparision
struct comp {
    bool operator ()(const QueueNode a, const QueueNode b) {
        return strcmp(a.element, b.element);
    }
};

//  open file
FILE* openFile(char* fileName, char mode) {
    FILE* fp = fopen(fileName, &mode);
    if (fp == NULL) {
        perror("ERROR while opening the file");
        exit(EXIT_FAILURE);
    }
    return fp;
}
//  split file
void splitFile(char* inFile, int ramSize) {
    FILE* in = openFile(inFile, 'r');
    char fileName[5];
    
    //  initialize pointer to output files
    std::vector<FILE*> out (1, 0);

    //  initialize buffer
    char* buffer = (char*)malloc(ramSize * sizeof(char*));
    if (buffer == NULL) {
        perror("ERROR while malloc buffer");
        exit(EXIT_FAILURE);
    }
    //  track number of file created
    int outFile = 0;
    
    //  split big file to sorted chunks
    size_t nread;
    while (((nread = fread(buffer, 1, ramSize, in)) > 0)) {
        
        //  check last character for complete line
        int offsetGap = 0;
        while (buffer[nread - offsetGap - 1] != '\n' && buffer[nread - offsetGap - 1] != EOF)
            offsetGap++;
        buffer[nread-offsetGap] = '\0';
        fseek(in, -offsetGap, SEEK_CUR);
        
        //  create output file and write to file
        out.resize(outFile + 1);
        
        //  convert outFile to fileName
        snprintf(fileName, sizeof(fileName), "%d", outFile);
        out[outFile] = openFile(fileName, 'w');
        fwrite(buffer, 1, nread, out[outFile]);
        outFile++;
        
        //  TO DO: sort lines in file
     
    };
    
    //  close all file descriptor
    for (int i = 0; i < outFile; i++) {
        fclose(out[i]);
    }
    fclose(in);
}
// merge file using merge K-sorted array

int main(int argc, const char * argv[]) {
    splitFile("/Users/Dat/workspace/External_sort/External_sort/english.txt", 100000);
    return 0;
}



