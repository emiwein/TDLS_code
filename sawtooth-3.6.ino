int a = 0;
void setup(){
    analogWriteResolution(12);

}

void loop(){
    analogWrite(A22, 3000);
    delayMicroseconds(1000);
    analogWrite(A22, 0);
    delayMicroseconds(500);
    int a=992;
    while(a < 2700){
        analogWrite(A21, a);
        delayMicroseconds(37);
        a++;
    }
    analogWrite(A21,0);
    a = 0;
    delayMicroseconds(14500);
    delayMicroseconds(18000);
    
}
