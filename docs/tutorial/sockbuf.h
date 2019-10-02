#ifndef SOCKBUF_H
#define SOCKBUF_H

class sockbuf : public filebuf {
  public:
    sockbuf() { }
    sockbuf(int fd) : filebuf(fd) { }
    virtual streampos sys_seek(streamoff, _seek_dir) { return streampos(); }
};

#endif // SOCKBUF_H
