#ifndef __CEPH_CLS_RBD_H
#define __CEPH_CLS_RBD_H

#include "include/types.h"
#include "include/buffer.h"
#include "common/Formatter.h"

/// information about our parent image, if any
struct cls_rbd_parent {
  int64_t pool;        ///< parent pool id
  string id;           ///< parent image id
  snapid_t snapid;     ///< parent snapid we refer to
  uint64_t overlap;    ///< portion of this image mapped onto parent

  /// true if our parent pointer information is defined
  bool exists() const {
    return snapid != CEPH_NOSNAP && pool >= 0 && id.length() > 0;
  }

  cls_rbd_parent() : pool(-1), snapid(CEPH_NOSNAP), overlap(0) {}

  void encode(bufferlist& bl) const {
    ENCODE_START(1, 1, bl);
    ::encode(pool, bl);
    ::encode(id, bl);
    ::encode(snapid, bl);
    ::encode(overlap, bl);
    ENCODE_FINISH(bl);
  }
  void decode(bufferlist::iterator& bl) {
    DECODE_START(1, bl);
    ::decode(pool, bl);
    ::decode(id, bl);
    ::decode(snapid, bl);
    ::decode(overlap, bl);
    DECODE_FINISH(bl);
  }
  void dump(Formatter *f) const {
    f->dump_int("pool", pool);
    f->dump_string("id", id);
    f->dump_unsigned("snapid", snapid);
    f->dump_unsigned("overlap", overlap);
  }
  static void generate_test_instances(list<cls_rbd_parent*>& o) {
    o.push_back(new cls_rbd_parent);
    cls_rbd_parent *t = new cls_rbd_parent;
    t->pool = 1;
    t->id = "foo";
    t->snapid = 3;
    o.push_back(t);
  }
};
WRITE_CLASS_ENCODER(cls_rbd_parent)

struct cls_rbd_snap {
  snapid_t id;
  string name;
  uint64_t image_size;
  uint64_t features;
  cls_rbd_parent parent;

  /// true if we have a parent
  bool has_parent() const {
    return parent.exists();
  }

  cls_rbd_snap() : id(CEPH_NOSNAP), image_size(0), features(0) {}
  void encode(bufferlist& bl) const {
    ENCODE_START(2, 1, bl);
    ::encode(id, bl);
    ::encode(name, bl);
    ::encode(image_size, bl);
    ::encode(features, bl);
    ::encode(parent, bl);
    ENCODE_FINISH(bl);
  }
  void decode(bufferlist::iterator& p) {
    DECODE_START(2, p);
    ::decode(id, p);
    ::decode(name, p);
    ::decode(image_size, p);
    ::decode(features, p);
    if (struct_v >= 2) {
      ::decode(parent, p);
    }
    DECODE_FINISH(p);
  }
  void dump(Formatter *f) const {
    f->dump_unsigned("id", id);
    f->dump_string("name", name);
    f->dump_unsigned("image_size", image_size);
    f->dump_unsigned("features", features);
    if (has_parent()) {
      f->open_object_section("parent");
      parent.dump(f);
      f->close_section();
    }
  }
  static void generate_test_instances(list<cls_rbd_snap*>& o) {
    o.push_back(new cls_rbd_snap);
    cls_rbd_snap *t = new cls_rbd_snap;
    t->id = 1;
    t->name = "snap";
    t->image_size = 123456;
    t->features = 123;
    o.push_back(t);
    t = new cls_rbd_snap;
    t->id = 2;
    t->name = "snap2";
    t->image_size = 12345678;
    t->features = 1234;
    t->parent.pool = 1;
    t->parent.id = "parent";
    t->parent.snapid = 456;
    t->parent.overlap = 12345;
    o.push_back(t);
  }
};
WRITE_CLASS_ENCODER(cls_rbd_snap)

#endif
