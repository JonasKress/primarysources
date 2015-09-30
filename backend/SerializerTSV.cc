// Copyright 2015 Google Inc. All Rights Reserved.
// Author: Sebastian Schaffert <schaffert@google.com>

#include "SerializerTSV.h"


namespace Serializer {

    static void writeValueTSV(const Value& v, std::ostream* out) {
        switch (v.getType()) {
            case ENTITY:
                *out << v.getString();
                break;
            case LOCATION:
                *out << "@" << v.getLocation().first
                     << "/" << v.getLocation().second;
                break;
            case QUANTITY:
                *out << v.getQuantityAsString();
                break;
            case STRING:
                if (v.getLanguage() != "") {
                    *out << v.getLanguage() << ":";
                }
                out->put('"');
                for (char c : v.getString()) {
                    if (c == '\\' || c == '"') {
                        out->put('\\');
                    }
                    out->put(c);
                }
                out->put('"');
                break;
            case TIME:
                Time time = v.getTime();
                *out << std::setfill('0')
                        << "+" << std::setw(4) << time.year
                        << "-" << std::setw(2) << time.month
                        << "-" << std::setw(2) << time.day
                        << "T" << std::setw(2) << time.hour
                        << ":" << std::setw(2) << time.minute
                        << ":" << std::setw(2) << time.second
                        << "Z/" << v.getPrecision();
                break;
        }
    }


    void writeStatementTSV(const Statement& stmt, std::ostream* out) {
        *out << stmt.getQID() << "\t"
             << stmt.getProperty() << "\t";
        writeValueTSV(stmt.getValue(), out);

        for (const PropertyValue& pv : stmt.getQualifiers()) {
            *out << "\t" << pv.getProperty() << "\t";
            writeValueTSV(pv.getValue(), out);
        }

        for (const PropertyValue& pv : stmt.getSources()) {
            *out << "\t" << pv.getProperty() << "\t";
            writeValueTSV(pv.getValue(), out);
        }
        *out << std::endl;
    }

    void writeStatementEnvelopeJSON(
            const Statement& stmt, cppcms::json::value* out) {
        cppcms::json::value entity;

        // write statement as TSV to a string value
        std::ostringstream sout;

        sout << stmt.getQID() << "\t"
             << stmt.getProperty() << "\t";
        writeValueTSV(stmt.getValue(), &sout);

        for (const PropertyValue& pv : stmt.getQualifiers()) {
            sout << "\t" << pv.getProperty() << "\t";
            writeValueTSV(pv.getValue(), &sout);
        }

        for (const PropertyValue& pv : stmt.getSources()) {
            sout << "\t" << pv.getProperty() << "\t";
            writeValueTSV(pv.getValue(), &sout);
        }

        (*out)["statement"] = sout.str();
        (*out)["id"] = stmt.getID();
        (*out)["format"] = "v1";
        (*out)["dataset"] = stmt.getDataset();
        (*out)["upload"] = stmt.getUpload();
        (*out)["state"] = stateToString(stmt.getApprovalState());
    }

}  // namespace Serializer
