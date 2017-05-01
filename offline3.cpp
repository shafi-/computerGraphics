#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>
#include<ctime>
#include "bitmap_image.hpp"

using namespace std;

#define MIN .000000001
/**
 * structures
 */
struct color{
    int r,g,b;
};

struct point{
    float a,b,c;
    bool input(){ bool isInput = cin>>a>>b>>c; return isInput;}
    void print(){ cout<<"("<<a<<","<<b<<","<<c<<")"<<endl;}
};

struct triangle{
    point points[3];
    color c;
    void input()
    {
        points[0].input();
        points[1].input();
        points[2].input();
    }
    void print()
    {
        cout<<"{";
        points[0].print();
        points[1].print();
        points[2].print();
        cout<<"}"<<endl;
    }
};


/**
 * variables
 */

float dx, dy;
float topY, leftX;
float** z_buffer;
color** frame_buffer;
float screen_width;
float screen_height;
float limitX, limitY, front_, rear;
vector<triangle> triangles;
//bitmap_image* image;
float LEFT_INTERSECTING_COLUMN;
float RIGHT_INTERSECTING_COLUMN;
float TOP_SCANLINE;
float BOTTOM_SCANLINE;
/**
 * methods
 */

void read_data();
void initialize_z_buffer_and_frame_buffer();
void getTopBottom(triangle t);
void getLeftRight(triangle t, float row);
double calculateZ(triangle t, float row, float col, float xa, float xb);
void apply_procedure();
void save();
void free_memory();
void config();
void test();

float min2(float a, float b)
{
    return a>b?b:a;
}

float max2(float a, float b)
{
    return a>b?a:b;
}

float min3(float a, float b, float c)
{
    return min2(a, min2(b,c));
}

float max3(float a, float b, float c)
{
    return max2(a, max2(b,c));
}

int main()
{
//    test();
//    return 0;
    read_data();
    initialize_z_buffer_and_frame_buffer();
    apply_procedure();
    save();
    free_memory();
    return 0;
}

void read_data()
{
    FILE* f = freopen("config.txt","r",stdin);
    //cout<<"Read config..."<<endl;
    cin>>screen_width>>screen_height;
    cin>>limitX;
    cin>>limitY;
    cin>>front_>>rear;
    fclose(f);

    //config();

    point p;
    triangle t;
    srand(0);
    f = freopen("scene3.txt","r",stdin);
    while(p.input())
    {
        t.points[0] = p;
        p.input();
        t.points[1] = p;
        p.input();
        t.points[2] = p;
        t.c.r = 100+rand()%156;
        t.c.g = 100+rand()%156;
        t.c.b = 100+rand()%156;
        triangles.push_back(t);
    }
    fclose(f);
    for(int i=0; i< triangles.size(); i++)
    {
        //triangles[i].print();
    }
    freopen("frame_buffer.txt","w",stdout);
}

void initialize_z_buffer_and_frame_buffer()
{
    dx = 2*limitX/screen_width; dx = dx > 0 ? dx : -dx;
    dy = 2*limitY/screen_height; dy = dy > 0 ? dy : -dy;

    topY = abs(limitY) - dy/2;
    leftX = -abs(limitX) + dx/2;

    z_buffer = (float**) malloc(sizeof(float)*(screen_height+5));
    for(int i=0; i<screen_height + 4; i++)
    {
        z_buffer[i] = (float*) malloc(sizeof(float)*(screen_width+5));

        for(int j=0; j<screen_width + 4; j++)
        {
            z_buffer[i][j] = rear;
        }
    }

    frame_buffer = (color**) malloc(sizeof(color)*(screen_height+5));

    for(int i=0; i<screen_height + 4; i++)
    {
        frame_buffer[i] = (color*) malloc(sizeof(color)*(screen_width+5));

        for(int j=0; j<screen_width + 4; j++)
        {
            frame_buffer[i][j].r = 0;
            frame_buffer[i][j].g = 0;
            frame_buffer[i][j].b = 0;
        }
    }
//    z_buffer = (float**) new float sizeof(float)*screen_height*screen_width;
    //image = new bitmap_image(screen_width,screen_height);
}

void getTopBottom(triangle t)
{
    TOP_SCANLINE = max3(t.points[0].b, t.points[1].b, t.points[2].b);
    BOTTOM_SCANLINE = min3(t.points[0].b, t.points[1].b, t.points[2].b);

    if(TOP_SCANLINE > -limitY){
        TOP_SCANLINE = -limitY;
    }
    if(BOTTOM_SCANLINE < limitY)
    {
        BOTTOM_SCANLINE = limitY;
    }
}

float rowLineIntersection(float row, point p1, point p2)
{
    float x = p1.a + (p1.a - p2.a) * (row - p1.b) / (p1.b - p2.b) ;
    return x;
}


void test()
{
    triangle t;
    t.points[0].a = 10;
    t.points[0].b = 0;
    t.points[1].a = 20;
    t.points[1].b = 0;
    t.points[2].a = 15;
    t.points[2].b = 10;
    for(int i=0; i<25; i++){
        //cout<<i<<" :: "<<rowLineIntersection(i, t.points[0], t.points[2])<<endl;
        getLeftRight(t, i);
    }
}
void getLeftRight(triangle t, float row)
{
    //set LEFT_INTERSECTING_COLUMN, RIGHT_INTERSECTING_COLUMN LEFT_INTERSECTING_COLUMN = RIGHT_INTERSECTING_COLUMN
    //check l1(0,1), l2(1,2), l3(2,0) for y = row
    float maxX, minX, x1, x2;
    maxX = max3(t.points[0].a, t.points[1].a, t.points[2].b);
    minX = min3(t.points[0].a, t.points[1].a, t.points[2].b);

    /* new */

    LEFT_INTERSECTING_COLUMN = maxX;
    RIGHT_INTERSECTING_COLUMN = minX;

    //cout<<"MinX: "<<minX<<" MaxX: "<<maxX<<endl;


    x1 = rowLineIntersection(row, t.points[0], t.points[1]);
    if( minX <= x1 && x1 <= maxX ){
        //update left,right
        //cout<<"intersected 0 - 1 at "<<x1<<"\n";
        if(x1 < LEFT_INTERSECTING_COLUMN) LEFT_INTERSECTING_COLUMN = x1;
        if(RIGHT_INTERSECTING_COLUMN < x1) RIGHT_INTERSECTING_COLUMN = x1;
    }

    x1 = rowLineIntersection(row, t.points[1], t.points[2]);
    if( minX <= x1 && x1 <= maxX ){
        //update left,right
        //cout<<"intersected 1 - 2 at "<<x1<<"\n";
        if(x1 < LEFT_INTERSECTING_COLUMN) LEFT_INTERSECTING_COLUMN = x1;
        if(RIGHT_INTERSECTING_COLUMN < x1) RIGHT_INTERSECTING_COLUMN = x1;
    }

    x1 = rowLineIntersection(row, t.points[2], t.points[0]);
    if( minX <= x1 && x1 <= maxX ){
        //update left,right
        //cout<<"intersected 2 - 0 at "<<x1<<"\n";
        if(x1 < LEFT_INTERSECTING_COLUMN) LEFT_INTERSECTING_COLUMN = x1;
        if(RIGHT_INTERSECTING_COLUMN < x1) RIGHT_INTERSECTING_COLUMN = x1;
    }
    //cout<<"scanline: "<<row<<" :: "<<LEFT_INTERSECTING_COLUMN<<" , "<<RIGHT_INTERSECTING_COLUMN<<endl;

}

double calculateZ(triangle t, float row, float col, float xa, float xb)
{
    /* plane geometry method */
    float a,b,c,d;

    float zp;

    a = (t.points[1].b - t.points[0].b)*(t.points[2].c - t.points[0].c) - (t.points[2].b - t.points[0].b)*(t.points[1].c - t.points[0].c);
    b = (t.points[1].c - t.points[0].c)*(t.points[2].a - t.points[0].a) - (t.points[2].c - t.points[0].c)*(t.points[1].a - t.points[0].a);
    c = (t.points[1].a - t.points[0].a)*(t.points[2].b - t.points[0].b) - (t.points[2].a - t.points[0].a)*(t.points[1].b - t.points[0].b);

    d = a*t.points[0].a + b*t.points[0].b + c*t.points[0].c;

    zp = (d - a*col - b*row)/c;
    return zp;
    /* line co-ordinate geometry method */
/*
    if( t.points[0].b < MIN && t.points[1].b < MIN && t.points[2].b < MIN) return t.points[0].c;

    float div_y1 = ( abs(t.points[0].b - t.points[1].b) > MIN ? (t.points[0].b - t.points[1].b) : t.points[0].b );
    float div_y2 = ( abs(t.points[0].b - t.points[2].b) > MIN ? (t.points[0].b - t.points[2].b) : t.points[0].b );

    double za , zb, zp;
    za = (t.points[0].b - row);
    za /= div_y1;
    za *= (t.points[0].c - t.points[1].c);
    za = t.points[0].c - za;

    zb = t.points[0].b - row;
    zb /= div_y2;
    zb *= t.points[0].c - t.points[2].c;
    zb = t.points[0].c - zb;

    zp = (zb - za)*(xb - col);
    zp /= (xb - xa);
    zp = zb - zp;
    //zp = zb - (zb - za)*(xb - col)/(xb - xa);
    printf("%f  %f  %f  %f = %f\n",row,div_y1, div_y2, col,zp);
*/

    return zp;
}

void apply_procedure()
{
    int t = triangles.size();

    TOP_SCANLINE = screen_height;
    BOTTOM_SCANLINE = 0;
    LEFT_INTERSECTING_COLUMN = 0;
    RIGHT_INTERSECTING_COLUMN = screen_width;

    float top_scanline, bottom_scanline, left_intersecting_column, right_intersecting_column;

    float z;

    for(int i=0; i< t; i++)
    {
        getTopBottom(triangles[i]);

        top_scanline = TOP_SCANLINE;
        bottom_scanline = BOTTOM_SCANLINE;

//        cout<<"For "<<i<<"th triangle: "<<TOP_SCANLINE<<","<<BOTTOM_SCANLINE<<endl;

        for(float row = top_scanline; row > bottom_scanline; row -= dy)
        {
            getLeftRight(triangles[i],row);

            left_intersecting_column = LEFT_INTERSECTING_COLUMN;
            right_intersecting_column = RIGHT_INTERSECTING_COLUMN;

            for(float col = left_intersecting_column; col< right_intersecting_column; col += dx)
            {
                z = calculateZ(triangles[i], row, col, left_intersecting_column, right_intersecting_column);
                //z = triangles[i].points[0].c;
                int r,c;

                r = (int) floor(row/dy);
                c = (int) floor(col/dx);

                r = r + screen_height/2;
                c = c + screen_width/2;
                //cout<<"{"<<r<<","<<c<<"}"<<endl;

                if( front_ <= z && z <= z_buffer[r][c])
                {
                    frame_buffer[r][c] = triangles[i].c;
                    z_buffer[r][c] = z;
                    //cout<<"("<<r<<","<<c<<") = ("<<triangles[i].c.r<<","<<triangles[i].c.g<<","<<triangles[i].c.b<<")"<<endl;
                }
            }
        }
    }
}

void save()
{
    bitmap_image image(screen_width, screen_height);
    for(int i=0;i<screen_height;i++){
        for(int j=0;j<screen_width;j++){
            image.set_pixel(j, screen_height - i, frame_buffer[i][j].r,frame_buffer[i][j].g, frame_buffer[i][j].b);
//            cout<<"("<<i<<","<<j<<") = ("<<frame_buffer[i][j].r<<","<<frame_buffer[i][j].g<<","<<frame_buffer[i][j].b<<")"<<endl;
        }
    }
    image.save_image("output.bmp");
    //free(image);
}

void free_memory()
{
    z_buffer = NULL;
    free(z_buffer);

    frame_buffer = NULL;
    free(frame_buffer);
    //image = NULL;
    //free(image);
}

void config()
{
    cout<<"Config: "<<endl;
    cout<<screen_width<<" x "<<screen_height<<endl;
    cout<<limitX<<" <=X<= " << -limitX<<endl;
    cout<<limitY<<" <=Y<= " << -limitY<<endl;
    cout<<front_<<" <=Z<= " << rear<<endl;
}
