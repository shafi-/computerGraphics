#include<iostream>
#include<vector>
#include<stack>
#include<string>

using namespace std;

struct point
{
    float a,b,c;
};

struct matrix
{
    float m[4][4];
    matrix operator* (const matrix& mat) const
    {
        matrix product;
        for(int i= 0; i<4; i++)
        {
            for(int k=0; k < 4; k++)
            {
                product.m[i][k] = 0;
                for(int j=0; j<4; j++)
                {
                    product.m[i][k] += m[i][k]*mat.m[j][k];
                }
            }
        }
        return product;
    }
};

vector<point> points;
stack<matrix> matrixStack;
stack<matrix> saveMatrix;


matrix rotationMatrix(float angle, point axis);
matrix translationMatrix(point translate);
matrix scaleMatrix(float scale);

void modelTransformation();
void viewTransformation();
void projectionTransformation();

void input()
{
    //glLookAt
    //projection vector
    string command;
    point p;
    // rotation
    float angle, scale;
    point axis;
    int endInput = 1;

    //transformation matrix
    matrix transformation;
    while(endInput && cin>>command)
    {

        if(command == "triangle")
        {
            cin>>p.a>>p.b>>p.c;
            points.push_back(p);
            cin>>p.a>>p.b>>p.c;
            points.push_back(p);
            cin>>p.a>>p.b>>p.c;
            points.push_back(p);
        }
        else if(command == "rotate")
        {
            cin>>angle;
            cin>>axis.a>>axis.b>>axis.c;
            transformation = rotationMatrix(angle,axis);
            transformation = matrixStack.top()*transformation;
            matrixStack.push(transformation);
        }
        else if(command == "translate")
        {
            cin>>axis.a>>axis.b>>axis.c;
            transformation = translationMatrix(axis);
            transformation = matrixStack.top()*transformation;
            matrixStack.push(transformation);
        }
        else if(command == "push")
        {
            saveMatrix.push(matrixStack.top());
        }
        else if(command == "pop")
        {
            saveMatrix.pop();
        }
        else if( command == "end")
        {
            endInput = 0;
        }
        else if(command == "scale")
        {
            cin>>scale;
            transformation = scaleMatrix(scale);
            transformation = matrixStack.top()*transformation;
            matrixStack.push(transformation);
        }
        else
        {
            cout<<"No match found for "<<command<<endl;
        }
    }
}


int main()
{
//    input();
    modelTransformation();
    viewTransformation();
    projectionTransformation();
    return 0;
}

matrix rotationMatrix(float angle, point axis){
    matrix mat;
    return mat;
}

matrix translationMatrix(point translate){
    matrix mat;
    return mat;
}

matrix scaleMatrix(float scale){
    matrix mat;
    return mat;
}

void modelTransformation()
{

}
void viewTransformation()
{

}
void projectionTransformation()
{

}
