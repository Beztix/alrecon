#ifndef BLOB_H
#define BLOB_H



namespace blob {

	void FindBlobs(const cv::Mat &binary, std::vector<std::vector<cv::Point2i>> &blobs);


}


#endif /* BLOB_H*/
