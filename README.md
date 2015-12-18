### 简单的PHP7 opcode查看器

PHP有一个很好的opcode查看器[VLD](https://github.com/derickr/vld)。不过代码比较复杂，不易看懂，为了方便理解，写了这个简单的查看器。

代码很简单，基本原理是使用扩展的方式在RINIT的时候修改zend_compile_file函数，将其编译出的opcode文件存放在php文件同目录下，
容易看清opcode存储的基本原理。

支持显示的格式不多，可以根据自己需要添加。

#### 使用方法
编译： phpize  && ./configure --prefix=$PHP_PREFIX && make install

详细参见 [Building extensions using phpize](http://www.phpinternalsbook.com/build_system/building_extensions.html#building-extensions-using-phpize)
