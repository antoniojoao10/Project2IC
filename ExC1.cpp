#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>

#include "Golomb.cpp"
#include "BitStream.cpp"

using namespace cv;

int m = 10000; // m value to use with Golomb class
int numYBits = 0;
int numUBits = 0;
int numVBits = 0;

int predictor(int pred1, int pred2, int pred3) {
    if (pred3 >= max(pred1, pred2))
        return min(pred1, pred2);

    else if (pred3 <= min(pred1, pred2))
        return max(pred1, pred2);

    else
        return pred1 + pred2 - pred3;
}

void encode_image(Mat originalImage, Mat yuvImage, string fileName[]) {
    vector<int> yChannel, uChannel, vChannel, outputF;

    // Iterate cols and rows
    for (int idxCols = 0; idxCols < yuvImage.cols; idxCols++) 
        for (int idxRows = 0; idxRows < yuvImage.rows; idxRows++) {     
            bool cond1 = (idxCols == 0);
            bool cond2 = (idxRows == 0 || idxRows == originalImage.rows || idxRows == yuvImage.rows - (originalImage.rows / 4));

            int pred1;
            if (cond1) 
                pred1 = 0;
            else
                pred1 = yuvImage.at<uchar>(idxCols - 1, idxRows);

            int pred2;
            if (cond2)
                pred2 = 0;
            else
                pred2 = yuvImage.at<uchar>(idxCols, idxRows - 1);

            int pred3;
            if (cond1 || cond2)
                pred3 = 0;
            else
                pred3 = yuvImage.at<uchar>(idxCols - 1, idxRows - 1);

            int e =  yuvImage.at<uchar>(idxCols, idxRows);
            if (idxRows < originalImage.rows) {
                int encode = e - predictor(pred1, pred2, pred3);
                yChannel.push_back(encode);
                outputF.push_back(encode);
            } else if (idxRows < yuvImage.rows - (originalImage.rows / 4)) {
                int encode = e - predictor(pred1, pred2, pred3);
                uChannel.push_back(encode);
                outputF.push_back(encode);
            } else {
                int encode = e - predictor(pred1, pred2, pred3);
                vChannel.push_back(encode);
                outputF.push_back(encode);
            }
        }

    // Recover size
    // Y Channel
    yChannel.insert(yChannel.begin(), originalImage.cols);
    yChannel.insert(yChannel.begin(), originalImage.rows);

    // U Channel
    uChannel.insert(uChannel.begin(), originalImage.cols);
    uChannel.insert(uChannel.begin(), originalImage.rows / 4);
    
    // V Channel
    vChannel.insert(vChannel.begin(), originalImage.cols); 
    vChannel.insert(vChannel.begin(), originalImage.rows / 4);
    

    // Golomb and BitStream
    // Y Channel
    Golomb golY;
    BitStream bsY {fileName[0], 'w'};
    for (int i=0; i<yChannel.size(); i++) {
        string golResString = golY.encoder(m, yChannel.at(1));

        int golResInt;
        stringstream aux;  
        aux << golResString;  
        aux >> golResInt;

        numYBits = golResString.length();

        bsY.writeBit(golResInt);
    }
    
    // U Channel
    Golomb golU;
    BitStream bsU {fileName[1], 'w'};
    for (int i=0; i<uChannel.size(); i++) {
        string golResString = golU.encoder(m, uChannel.at(1));

        int golResInt;
        stringstream aux;  
        aux << golResString;  
        aux >> golResInt;

        numUBits = golResString.length();

        bsU.writeBit(golResInt);
    }
    
    // V Channel
    Golomb golV;
    BitStream bsV {fileName[2], 'w'};
    for (int i=0; i<vChannel.size(); i++) {
        string golResString = golV.encoder(m, vChannel.at(1));

        int golResInt;
        stringstream aux;  
        aux << golResString;  
        aux >> golResInt;

        numVBits = golResString.length();

        bsV.writeBit(golResInt);
    }
}

void decode_image(Mat yuvImage, string fileName[]) {
    // Get bits from files and decode ---- DOES NOT WORK -----
    // Y Channel
    Golomb golY;
    BitStream bsY {fileName[0], 'r'};
    string golYRes = bsY.readNBits(numYBits);
    vector<int> yChannel;
    yChannel.push_back(golY.decoder(m, golYRes));

    // U Channel
    Golomb golU;
    BitStream bsU {fileName[0], 'r'};
    string golURes = bsU.readNBits(numUBits);
    vector<int> uChannel;
    uChannel.push_back(golU.decoder(m, golURes));

    // V Channel
    Golomb golV;
    BitStream bsV {fileName[0], 'r'};
    string golVRes = bsV.readNBits(numVBits);
    vector<int> vChannel;
    vChannel.push_back(golV.decoder(m, golVRes));


    // Iterate cols and rows
    int cols = vChannel[1]; 
    int rows = yChannel[0] + uChannel[0] + vChannel[0];
    int idx = 0;

    for (int idxCols = 0; idxCols < yuvImage.cols; idxCols++) 
        for (int idxRows = 0; idxRows < yuvImage.rows; idxRows++)
            if (idxRows < yChannel[0]) {
                bool cond1 = idx < yChannel[0];
                bool cond2 = idx % yChannel[0] == 0;

                int pred1;
                if (cond1)
                    pred1 = 0;
                else
                    pred1 = yuvImage.at<uchar>(idxCols - 1, idxRows);

                int pred2;
                if (cond2)
                    pred2 = 0;
                else
                    pred2 = yuvImage.at<uchar>(idxCols, idxRows - 1);

                int pred3;
                if (cond1 || cond2)
                    pred3 = 0;
                else
                    pred3 = yuvImage.at<uchar>(idxCols - 1, idxRows - 1);

                yuvImage.at<uchar>(idxCols, idxRows) = predictor(pred1, pred2, pred3) + yChannel.at(idx++ + 2);

            } else if (idxRows < yChannel[0] + uChannel[0]) {
                bool cond1 = idx < uChannel[0];
                bool cond2 = idx % uChannel[0] == 0;

                int pred1;
                if (cond1)
                    pred1 = 0;
                else
                    pred1 = yuvImage.at<uchar>(idxCols - 1, idxRows);

                int pred2;
                if (cond2)
                    pred2 = 0;
                else
                    pred2 = yuvImage.at<uchar>(idxCols, idxRows - 1);

                int pred3;
                if (cond1 || cond2)
                    pred3 = 0;
                else
                    pred3 = yuvImage.at<uchar>(idxCols - 1, idxRows - 1);

                yuvImage.at<uchar>(idxCols, idxRows) = predictor(pred1, pred2, pred3);

            } else {
                bool cond1 = idx < vChannel[0];
                bool cond2 = idx % vChannel[0] == 0;

                int pred1;
                if (cond1)
                    pred1 = 0;
                else
                    pred1 = yuvImage.at<uchar>(idxCols - 1, idxRows);

                int pred2;
                if (cond2)
                    pred2 = 0;
                else
                    pred2 = yuvImage.at<uchar>(idxCols, idxRows - 1);

                int pred3;
                if (cond1 || cond2)
                    pred3 = 0;
                else
                    pred3 = yuvImage.at<uchar>(idxCols - 1, idxRows - 1);

                yuvImage.at<uchar>(idxCols, idxRows) = predictor(pred1, pred2, pred3);
            }

    // Write new image
    Mat finalImage;
    cvtColor(yuvImage, finalImage, COLOR_YUV2BGR_I420);
    imwrite("new_image.ppm", finalImage);
    waitKey(0);
} 

int main(int argc, char **argv) {
    // Get original image
    string originalImageName = argv[1];
    Mat originalImage = imread(originalImageName, IMREAD_COLOR);
    
    // Transform the image into the YUV 4:2:0 format
    Mat yuvImage;
    cvtColor(originalImage, yuvImage, COLOR_BGR2YUV_I420);

    // File names BitStream will use
    string fileName[] = {"yChannelEncode.bits", "uChannelEncode.bits", "vChannelEncode.bits"};

    // Encode and decode images
    encode_image(originalImage, yuvImage, fileName);
    decode_image(yuvImage, fileName);

    return 0;
}