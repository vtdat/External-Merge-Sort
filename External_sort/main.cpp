//
//  main.cpp
//  External_sort
//
//  Created by Vu Tuan Dat on 8/11/18.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iterator>
#include <queue>

using namespace std;
class External_Sort
{
    private:
    int numChunks = 0;
    string inFileName = "";
    long ramSize = 0;
    string outFileName = "";
    //  constructor
    public: External_Sort(string inFile, string outFile, long size){
        inFileName = inFile;
        outFileName = outFile;
        ramSize = size;
    };
    
    // Queue node to be put in priority queue
    struct QueueNode {
        // line stored
        string element;
        // index of file
        int index;
    };

    // comparision
    struct comp {
        bool operator ()(const QueueNode a, const QueueNode b) {
            return (a.element).compare(b.element);
        }
    };

    //  split file
    public: void splitFile() {
        ifstream in;
        in.open(inFileName);
        string tempFile;
        
        //  track number of file created
        int numOutFile = 0;

        //  split big file to sorted chunks
        while (!in.eof()) {
            //  initialize buffer size
            long buffSize = 0;
            
            //  initialize vector to store line loaded
            vector<string> lines;
            string line;
            while (getline(in, line) && buffSize < ramSize) {
                lines.push_back(line+"\n");
                buffSize += (int)line.length();
            };
            
            //  sort vector of string
            sort(lines.begin(), lines.end());
            
            //  convert outFile to fileName
            tempFile = to_string(numOutFile);
            //  copy buffer to chunk
            ofstream outStream (tempFile);
            ostream_iterator<string> output_iterator(outStream);
            copy(lines.begin(), lines.end(), output_iterator);
            
            //  close file descriptor
            outStream.close();
            numOutFile++;
        };
        in.close();
        numChunks = numOutFile++;
    }
    
    // merge file using merge K-sorted array
    public: void mergeFiles () {
        ifstream inFiles[numChunks];
        string inName;
        
        //  open file to stream
        for (int i = 0; i < numChunks; i++) {
            inName = to_string(i);
            inFiles[i].open(inName);
        }
        
        
        //  create output stream
        ofstream out(outFileName);
        
        //  create queue
        QueueNode nodes[numChunks];
        
        //  create priority queue
        priority_queue<QueueNode, vector<QueueNode>, comp> pq;
        
        //  read data to queue
        for (int i = 0; i < numChunks; i++) {
            if (!getline(inFiles[i], nodes[i].element))
                break;
            
            nodes[i].index = i;
            pq.push(nodes[i]);
        }
        
        //  count number of file reaching EOF
        int count = 0;
        
        while (count != numChunks) {
            
            //  put minimun element from priority queue to output
            QueueNode top = pq.top();
            pq.pop();
            out << top.element << "\n";
            
            //  put next element of input file containing last popped element
            if (!getline(inFiles[top.index], top.element)) {
                count++;
            } else {
                pq.push(top);
            }
        };
        
        //  close all input and output streams
        for (int i = 0; i < numChunks; i++)
            inFiles[i].close();
        out.close();
    }
    
};

int main(int argc, const char * argv[]) {
    if (argc != 4) {
        perror("Not enough arguments!");
        exit(EXIT_FAILURE);
    }
    External_Sort sort(argv[1], argv[2], atol(argv[3]));
    sort.splitFile();
    sort.mergeFiles();
    return 0;
}



