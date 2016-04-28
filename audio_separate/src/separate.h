/*
 * separate.h
 *
 *  Created on: Apr 28, 2016
 *      Author: jake
 */

#ifndef SRC_SEPARATE_H_
#define SRC_SEPARATE_H_

using namespace Eigen;
using namespace std;

MatrixXd separate(const MatrixXd in, const int buffer) {

	/* Separate Sound */
	const MatrixXd centered = in.colwise() - in.rowwise().mean(); // center data
	const MatrixXd cov = centered.transpose().adjoint() * centered.transpose() / buffer;  // normalized covariance
	const MatrixXd Y = cov.inverse().sqrt() * centered;  // Solve Y * cov = centered
	const MatrixXd sum = Y.colwise().sum().replicate(2,1);
	const MatrixXd combined = Y.cwiseProduct(sum) * Y.transpose();
	const JacobiSVD<MatrixXd> svd(combined, ComputeThinU | ComputeThinV);
	const MatrixXd U = svd.matrixU();  // unmixing
	const MatrixXd out = U * in;

	/* Debug Print */
	//  cout << "input =" << endl << in << endl;
	cout << "covariance =" << endl << cov << endl;
	//	cout << "Y =" << endl << Y << endl;
	cout << "unmix kernel =" << endl << U << endl;
	cout << "output =" << endl << out << endl;

	return out;
}

#endif /* SRC_SEPARATE_H_ */
