int array[262144];

void loop() {
  int i, j, offset;
  while(1) {
    for (j = 511; j >= 0; j--) {
    offset = j * 64;
    i = array[0 + offset];
    i = array[8192 + offset];
    i = array[16384 + offset];
    i = array[24576 + offset];

    }
    
  }
}

int main() {
  loop();
}
