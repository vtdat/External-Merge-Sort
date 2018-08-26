//
//  main.cpp
//  External_sort
//
//  Created by Vu Tuan Dat on 8/11/18.
//


// 		IDEA
//	1.	Split big file of N lines to K = (N/R) where K, R are number of chunks and Ram size (Input) respectively 
//	2.	Sort individual chunk using std::sort 
//	3.	Using Merge K-sorted array with priority_queue to merge K chunks to one output

//      COMPLEXITY
//  1.  O(N)            :   iterate N lines
//  2.  O(N*log(N/K))   :   sort K chunks, each chunk takes O((N/K)*log(N/K))
//  3.  O(N*l*log(N/K)) :   where l is average length of lines
//  => (1) and (2) are much smaller than (3) so we can consider it is approximate to O(N*l*log(N/K)) or O(fileSize * log(N/K))
//      since N(num of lines) * l(average length of lines) ~ fizeSize

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
    int numChunks;
    string inFileName;
    long ramSize;
    string outFileName;
    
    //  constructor
public:
    External_Sort(string inFile, string outFile, long size){
        inFileName = inFile;
        outFileName = outFile;
        ramSize = size;
    };
    
    // Queue node to be put in priority queue
private:
    struct QueueNode {
        // line stored
        string element;
        // index of file
        int index;
    };

    // comparision
private:
    struct Comp {
    public:
        bool operator ()(const QueueNode a, const QueueNode b) const {
            return (b.element).compare(a.element) < 0;
        }
    };
    
    //  check if file is empty
private:
    bool is_empty(ifstream& pFile)
    {
        return pFile.peek() == std::ifstream::traits_type::eof();
    }
    //  split file
public:
    void splitFile() {
        
        //  open input file
        ifstream in;
        in.open(inFileName);
        if(!in.good()){
            perror("Error while opening file");
            exit(EXIT_FAILURE);
        }
        
        //  check if file is empty
        if (is_empty(in)) {
            cout << "File " << inFileName <<" is empty!\n";
            exit(EXIT_SUCCESS);
        }
        
        string tempFile;
        
        //  track number of file created
        int numOutFile = 0;
        
        //  last line of previous chunk
        string lastLine;
        //  split big file to sorted chunks
        while (!in.eof()) {
            //  initialize buffer size
            long buffSize = 0;
            
            //  initialize vector to store line loaded
            vector<string> lines;
            
            //  put last line of previous chunk to list
            if (!lastLine.empty()) {
                lines.push_back(lastLine);
                lastLine.clear();
            }
            
            //  line from file
            string line;
            
            //  read file to list
            while (getline(in, line) && buffSize < ramSize) {
                buffSize += (int)line.length();
                if (buffSize >= ramSize) {
                    lastLine = line+"\n";
                    break;
                }
                lines.push_back(line+"\n");
            };
            
            //  sort list of string
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
public:
    void mergeFiles () {
        ifstream inFiles[numChunks];
        string inName;
        
        //  open file to stream
        for (int i = 0; i < numChunks; i++) {
            inName = to_string(i);
            inFiles[i].open(inName);
            if(!inFiles[i].good()){
                perror("Error while opening file");
                exit(EXIT_FAILURE);
            }
        }
        
        
        //  create output stream
        ofstream out(outFileName);
        
        //  create queue
        QueueNode nodes[numChunks];
        
        //  create priority queue
        priority_queue<QueueNode, vector<QueueNode>, Comp> pq;
        
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
        
        //  put remaining elements to output file
        while (!pq.empty()){
            out << pq.top().element << "\n";
            pq.pop();
        }
        
        //  close all input and output streams
        for (int i = 0; i < numChunks; i++){
            inFiles[i].close();
            remove(to_string(i).c_str());
        }
        out.close();
    }
    
};

int main(int argc, const char * argv[]) {
    if (argc != 4) {
        perror("Not enough arguments!\n");
        exit(EXIT_FAILURE);
    }
    External_Sort sort(argv[1], argv[2], atol(argv[3]));
    sort.splitFile();
    sort.mergeFiles();
    return 0;
}
