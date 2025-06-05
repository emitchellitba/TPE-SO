1. El memory manager utilizado por el kernel se puede modificar en tiempo de compilacion. Para ello el script compile.sh acepta
   un parametro. En caso de no especificarse, se defaultea al memory manager dummy.

   Para ejecutar el kernel con el DUMMY memory manager:

   ./compile.sh USE_SIMPLE_MM

  Para ejecutar el kernel con el BUDDY memory manager:

  ./compile.sh USE_BUDDY_MM
