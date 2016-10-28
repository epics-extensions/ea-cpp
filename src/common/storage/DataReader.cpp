// DataReader.cpp

// storage
#include "storage/DataReader.h"

DataReader::~DataReader()
{}

void DataReader::toString(stdString &text) const {
    const RawValue::Data *value = get();
    RawValue::toString(text, getType(), getCount(), value, &getInfo());
}

