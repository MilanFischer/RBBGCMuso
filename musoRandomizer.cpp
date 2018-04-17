#include <Rcpp.h>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <ctime>


using namespace Rcpp;
using namespace std;
// [[Rcpp::plugins(cpp11)]]



NumericMatrix randTypeZero(NumericMatrix m){
  int n=m.nrow()-1;
  NumericMatrix M(n+1,2);
  M(_,0)=m(_,0);
  for(int i=0;i<=n;++i){
    double min=m(i,2);
    double max=m(i,3);
    M(i,1)=runif(1,min,max)[0];
  }
  return M;
}

// [[Rcpp::export]]
NumericMatrix randTypeOne(NumericMatrix m){
  NumericVector dependence=m(_,2);
  int n=m.nrow()-1;
  NumericMatrix M(n+1,2);
  M(_,0)=m(_,0);
  M(0,1)=runif(1,m(0,2),m(0,3))[0];
  for(int i=1;i<=n;++i){
    int dep=m(i,1)-1;
    double min=max(M(dep,1),m(i,2));
    double max=m(i,3);
    M(i,1)=runif(1,min,max)[0];
  }
  return M;
}



IntegerVector orderDec(NumericVector v){
  Function f("order");
  return f(v,_["decreasing"]=1);
}

NumericMatrix randTypeTwo(NumericMatrix m){
  int n=m.nrow()-1;
  int N=n-1;
  NumericMatrix mv=m(Range(0,(n-1)),_);
  NumericVector dependence=m(_,2);
  NumericMatrix M(n+1,2);
  M(_,0)=m(_,0);
  IntegerVector indexes=orderDec(mv(_,2));
  NumericVector sorban=mv(_,2);
  sorban.sort(true);
  NumericVector sor=cumsum(sorban);
  sor.sort(true);
  for(int i=0;i<=N;++i){
    if(i!=N){
      mv((indexes[i]-1),3)-= sor[i+1];
    }
  }
  
  double rollingNumber=0;
  
  for(int i=0;i<=N;++i){
    double minimum=mv((indexes[i]-1),2);
    double maximum=mv((indexes[i]-1),3)-rollingNumber;
    M(i,1)=runif(1,minimum,maximum)[0];
    rollingNumber+=M(i,1);
    // cout << "minimum:\t" << minimum << endl;
    // cout << "maximum:\t" << maximum << endl;
    // cout << "indexes:\t" << indexes[i] << endl;
    // cout << "rollingNumber:\t" << rollingNumber << endl;
    // cout << "choosen:\t" << M(i,1) <<endl;
    // cout << "sor:\t" << sor <<endl;
    // cout << "\n\n\n" << mv;
  }
  M(n,1)=1-rollingNumber;
  return M;
}




NumericMatrix copyMatSpec(NumericMatrix A, NumericMatrix B, int u, int v){
  int k=0;
  for(int i=u;i<=v;++i){
    A(i,_)=B(k,_);
    k+=1;
  }
  return A;
}

//' musoRandomizer
//' 
//' This function is fastly randomize values based on min and max values,
//' and row indexes.
//' @importFrom Rcpp evalCpp
//' @useDynLib RBBGCMuso
//' @param A is the big matrix
//' @param B is the small matrix
//' @export
// [[Rcpp::export]]
NumericMatrix musoRandomizer(NumericMatrix A, NumericMatrix B){
  NumericMatrix M(A.nrow(),2);
  int nGroup = B.nrow()-1;
  int k=0; 
  for(int i=0;i<=nGroup;++i)
  {
    int b=B(i,0)-1;
    int till=b+k;
    int t=B(i,1);
    // cout << b << "\t" << t <<endl;
    switch(t){
    case 0:
      M=copyMatSpec(M,randTypeZero(A(Range(k,till),_)),k,till);
      // cout << M << endl;
      break;
    case 1:
      M=copyMatSpec(M,randTypeOne(A(Range(k,till),_)),k,till);
      // cout << M << endl;
      break;
    case 2:
      M=copyMatSpec(M,randTypeTwo(A(Range(k,till),_)),k,till);
      // cout << M << endl;
      break;
    }
    k=till+1;
    // cout << k << endl;
  }
  return M;
}

std::string concatenate(std::string A, std::string B){
  std::string C = A + B;
  return C;
}
