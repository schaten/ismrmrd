#include "ismrmrd/xml.h"
#include "ismrmrd/version.h"
#include "pugixml.hpp"
#include <cstdlib>
#include <iomanip>

namespace ISMRMRD {
long stoi(const char *str) {
    return atol(str);
}

long stoll(const char *str) {
    return atol(str);
}

float strtof(const char *str, char **str_end = NULL) {
    return static_cast<float>(atof(str));
}

float stof(const char *str, char **str_end = NULL) {
    return static_cast<float>(atof(str));
}

template <typename T>
std::string to_string(const T &v) {
    std::stringstream ss;
    ss.imbue(std::locale::classic());
    ss << std::setprecision(9) << v;
    return ss.str();
}

unsigned long stoul(const std::string &str) {
    return strtoul(str.c_str(), NULL, 10);
}

// Utility Functions for deserializing Header
EncodingSpace parse_encoding_space(pugi::xml_node &n, const char *child) {
    EncodingSpace e;
    pugi::xml_node encodingSpace = n.child(child);
    pugi::xml_node matrixSize = encodingSpace.child("matrixSize");
    pugi::xml_node fieldOfView_mm = encodingSpace.child("fieldOfView_mm");

    if (!matrixSize) {
        throw std::runtime_error("matrixSize not found in encodingSpace");
    } else {
        e.matrixSize.x = static_cast<uint16_t>(ISMRMRD::stoi(matrixSize.child_value("x")));
        e.matrixSize.y = static_cast<uint16_t>(ISMRMRD::stoi(matrixSize.child_value("y")));
        e.matrixSize.z = static_cast<uint16_t>(ISMRMRD::stoi(matrixSize.child_value("z")));
    }

    if (!fieldOfView_mm) {
        throw std::runtime_error("fieldOfView_mm not found in encodingSpace");
    } else {
        e.fieldOfView_mm.x = ISMRMRD::strtof(fieldOfView_mm.child_value("x"), nullptr);
        e.fieldOfView_mm.y = ISMRMRD::strtof(fieldOfView_mm.child_value("y"), nullptr);
        e.fieldOfView_mm.z = ISMRMRD::strtof(fieldOfView_mm.child_value("z"), nullptr);
    }

    return e;
}

Optional<Limit> parse_encoding_limit(pugi::xml_node &n, const char *child) {
    Optional<Limit> o;
    pugi::xml_node nc = n.child(child);

    if (nc) {
        Limit l;
        l.minimum = static_cast<uint16_t>(ISMRMRD::stoi(nc.child_value("minimum")));
        l.maximum = static_cast<uint16_t>(ISMRMRD::stoi(nc.child_value("maximum")));
        l.center = static_cast<uint16_t>(ISMRMRD::stoi(nc.child_value("center")));
        o = l;
    }

    return o;
}

std::string parse_string(pugi::xml_node &n, const char *child) {
    std::string r(n.child_value(child));
    if (r.size() == 0)
        throw std::runtime_error("Null length string");
    return r;
}

Optional<std::string> parse_optional_string(pugi::xml_node &n, const char *child) {
    std::string s(n.child_value(child));
    Optional<std::string> r;
    if (s.size())
        r = s;
    return r;
}

Optional<float> parse_optional_float(pugi::xml_node &n, const char *child) {
    Optional<float> r;
    pugi::xml_node nc = n.child(child);
    if (nc) {
        r = ISMRMRD::strtof(nc.child_value(), nullptr);
    }
    return r;
}

Optional<ISMRMRD::int64_t> parse_optional_long(pugi::xml_node &n, const char *child) {
    Optional<ISMRMRD::int64_t> r;
    pugi::xml_node nc = n.child(child);
    if (nc) {
        r = ISMRMRD::stoll(nc.child_value());
    }
    return r;
}

Optional<ISMRMRD::uint16_t> parse_optional_ushort(pugi::xml_node &n, const char *child) {
    Optional<ISMRMRD::uint16_t> r;
    pugi::xml_node nc = n.child(child);
    if (nc) {
        r = static_cast<ISMRMRD::uint16_t>(ISMRMRD::stoi(nc.child_value()));
    }
    return r;
}

float parse_float(pugi::xml_node &n, const char *child) {
    try {
        return ISMRMRD::stof(n.child_value(child), nullptr);
    } catch (const std::invalid_argument &) {
        throw std::runtime_error("Illegal value encountered in node " + std::string(n.name()) + ". Value is not a float:" + std::string(n.child_value(child)));

    } catch (const std::out_of_range &) {
        throw std::runtime_error("Value out of float range in node " + std::string(n.name()) + ". Value:" + std::string(n.child_value(child)));
    }
}

std::vector<float> parse_vector_float(pugi::xml_node &n, const char *child) {
    std::vector<float> r;

    pugi::xml_node nc = n.child(child);

    while (nc) {
        float f = ISMRMRD::strtof(nc.child_value(), nullptr);
        r.push_back(f);
        nc = nc.next_sibling(child);
    }

    return r;
}

std::vector<std::string> parse_vector_string(pugi::xml_node &n, const char *child) {
    std::vector<std::string> r;
    pugi::xml_node nc = n.child(child);
    while (nc) {
        std::string s = nc.child_value();
        r.push_back(s);
        nc = nc.next_sibling(child);
    }
    return r;
}

Optional<threeDimensionalFloat> parse_optional_threeDimensionalFloat(pugi::xml_node &n, const char *child) {

    Optional<threeDimensionalFloat> r;

    pugi::xml_node nc = n.child(child);
    if (nc) {
        threeDimensionalFloat s;
        s.x = ISMRMRD::strtof(nc.child_value("x"), nullptr);
        s.y = ISMRMRD::strtof(nc.child_value("y"), nullptr);
        s.z = ISMRMRD::strtof(nc.child_value("z"), nullptr);
        r = s;
    }

    return r;
}

std::vector<UserParameterLong> parse_user_parameter_long(pugi::xml_node &n, const char *child) {
    std::vector<UserParameterLong> r;
    pugi::xml_node nc = n.child(child);
    while (nc) {
        UserParameterLong v;
        pugi::xml_node name = nc.child("name");
        pugi::xml_node value = nc.child("value");

        if (!name || !value) {
            throw std::runtime_error("Malformed user parameter (long)");
        }

        v.name = std::string(name.child_value());
        v.value = ISMRMRD::stoi(value.child_value());

        r.push_back(v);

        nc = nc.next_sibling(child);
    }
    return r;
}

std::vector<UserParameterDouble> parse_user_parameter_double(pugi::xml_node &n, const char *child) {
    std::vector<UserParameterDouble> r;
    pugi::xml_node nc = n.child(child);
    while (nc) {
        UserParameterDouble v;
        pugi::xml_node name = nc.child("name");
        pugi::xml_node value = nc.child("value");

        if (!name || !value) {
            throw std::runtime_error("Malformed user parameter (double)");
        }

        char buffer[10000];
        memcpy(buffer, name.child_value(), strlen(name.child_value()) + 1);
        v.name = name.child_value();
        v.value = std::atof(value.child_value());

        r.push_back(v);

        nc = nc.next_sibling(child);
    }

    return r;
}

std::vector<UserParameterString> parse_user_parameter_string(pugi::xml_node &n, const char *child) {
    std::vector<UserParameterString> r;
    pugi::xml_node nc = n.child(child);
    while (nc) {
        UserParameterString v;
        pugi::xml_node name = nc.child("name");
        pugi::xml_node value = nc.child("value");

        if (!name || !value) {
            throw std::runtime_error("Malformed user parameter (string)");
        }

        v.name = std::string(name.child_value());
        v.value = std::string(value.child_value());

        r.push_back(v);

        nc = nc.next_sibling(child);
    }

    return r;
}
MultibandCalibrationType parse_multiband_type(const std::string &multibandString) {
    if (multibandString == "separable2D")
        return MultibandCalibrationType::SEPARABLE2D;
    if (multibandString == "full3D")
        return MultibandCalibrationType::FULL3D;
    if (multibandString == "other")
        return MultibandCalibrationType::OTHER;

    throw std::runtime_error("Invalid multiband calibration type in xml header: " + multibandString);
}

TrajectoryType parse_trajectory_type(const std::string &trajectoryString) {
    if (trajectoryString == "cartesian")
        return TrajectoryType::CARTESIAN;
    if (trajectoryString == "epi")
        return TrajectoryType::EPI;
    if (trajectoryString == "radial")
        return TrajectoryType::RADIAL;
    if (trajectoryString == "goldenangle")
        return TrajectoryType::GOLDENANGLE;
    if (trajectoryString == "spiral")
        return TrajectoryType::SPIRAL;
    if (trajectoryString == "other")
        return TrajectoryType::OTHER;

    throw std::runtime_error("Invalid trajectory type in xml header");
}

WaveformType parse_waveform_type(const std::string &waveformString) {
    if (waveformString == "ecg")
        return WaveformType::ECG;
    if (waveformString == "pulse")
        return WaveformType::PULSE;
    if (waveformString == "respiratory")
        return WaveformType::RESPIRATORY;
    if (waveformString == "trigger")
        return WaveformType::TRIGGER;
    if (waveformString == "gradientwaveform")
        return WaveformType::GRADIENTWAVEFORM;
    if (waveformString == "other")
        return WaveformType::OTHER;

    throw std::runtime_error("Invalid waveform type in xml header");
}

DiffusionDimension parse_diffusiondimension(const std::string &diffusiondimension) {
    if (diffusiondimension == "average")
        return DiffusionDimension::AVERAGE;
    if (diffusiondimension == "contrast")
        return DiffusionDimension::CONTRAST;
    if (diffusiondimension == "phase")
        return DiffusionDimension::PHASE;
    if (diffusiondimension == "repetition")
        return DiffusionDimension::REPETITION;
    if (diffusiondimension == "set")
        return DiffusionDimension::SET;
    if (diffusiondimension == "segment")
        return DiffusionDimension::SEGMENT;
    if (diffusiondimension == "user_0")
        return DiffusionDimension::USER_0;
    if (diffusiondimension == "user_1")
        return DiffusionDimension::USER_1;
    if (diffusiondimension == "user_2")
        return DiffusionDimension::USER_2;
    if (diffusiondimension == "user_3")
        return DiffusionDimension::USER_3;
    if (diffusiondimension == "user_4")
        return DiffusionDimension::USER_4;
    if (diffusiondimension == "user_5")
        return DiffusionDimension::USER_5;
    if (diffusiondimension == "user_6")
        return DiffusionDimension::USER_6;
    if (diffusiondimension == "user_7")
        return DiffusionDimension::USER_7;
    throw std::runtime_error("Invalid diffusion dimension in xml header");
}

static Diffusion parse_diffusion(pugi::xml_node &node) {
    Diffusion diff;
    diff.bvalue = ISMRMRD::stof(node.child_value("bvalue"));

    pugi::xml_node grad_node = node.child("gradientDirection");

    diff.gradientDirection.rl = ISMRMRD::stof(grad_node.child_value("rl"));
    diff.gradientDirection.ap = ISMRMRD::stof(grad_node.child_value("ap"));
    diff.gradientDirection.fh = ISMRMRD::stof(grad_node.child_value("fh"));
    return diff;
}

static Optional<std::vector<Diffusion> > parse_diffusion_vector(pugi::xml_node &node) {

    pugi::xml_node diffusion_node = node.child("diffusion");
    std::vector<Diffusion> diffusions;
    if (!diffusion_node)
        return diffusions;

    while (diffusion_node) {
        diffusions.push_back(parse_diffusion(diffusion_node));
        diffusion_node = diffusion_node.next_sibling("diffusion");
    }
    return diffusions;
}

// End of utility functions for deserializing header

void deserialize(const char *xml, IsmrmrdHeader &h) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml);

    if (!result) {
        throw std::runtime_error("Unable to load ISMRMRD XML header");
    }

    pugi::xml_node root = doc.child("ismrmrdHeader");

    if (root) {
        pugi::xml_node subjectInformation = root.child("subjectInformation");
        pugi::xml_node studyInformation = root.child("studyInformation");
        pugi::xml_node measurementInformation = root.child("measurementInformation");
        pugi::xml_node acquisitionSystemInformation = root.child("acquisitionSystemInformation");
        pugi::xml_node experimentalConditions = root.child("experimentalConditions");
        pugi::xml_node encoding = root.child("encoding");
        pugi::xml_node sequenceParameters = root.child("sequenceParameters");
        pugi::xml_node userParameters = root.child("userParameters");
        pugi::xml_node waveformInformation = root.child("waveformInformation");

        // Parsing version
        h.version = parse_optional_long(root, "version");

        // Parsing experimentalConditions
        if (!experimentalConditions) {
            throw std::runtime_error("experimentalConditions not defined in ismrmrdHeader");
        } else {
            ExperimentalConditions e;
            e.H1resonanceFrequency_Hz = std::atol(experimentalConditions.child_value("H1resonanceFrequency_Hz"));
            h.experimentalConditions = e;
        }

        // Parsing encoding section
        if (!encoding) {
            throw std::runtime_error("encoding section not found in ismrmrdHeader");
        } else {
            while (encoding) {
                Encoding e;

                try {
                    e.encodedSpace = parse_encoding_space(encoding, "encodedSpace");
                    e.reconSpace = parse_encoding_space(encoding, "reconSpace");
                } catch (std::runtime_error &e) {
                    std::cout << "Unable to parse encoding section: " << e.what() << std::endl;
                    throw;
                }

                pugi::xml_node encodingLimits = encoding.child("encodingLimits");

                if (!encodingLimits) {
                    throw std::runtime_error("encodingLimits not found in encoding section");
                } else {
                    e.encodingLimits.kspace_encoding_step_0 = parse_encoding_limit(encodingLimits, "kspace_encoding_step_0");
                    e.encodingLimits.kspace_encoding_step_1 = parse_encoding_limit(encodingLimits, "kspace_encoding_step_1");
                    e.encodingLimits.kspace_encoding_step_2 = parse_encoding_limit(encodingLimits, "kspace_encoding_step_2");
                    e.encodingLimits.average = parse_encoding_limit(encodingLimits, "average");
                    e.encodingLimits.slice = parse_encoding_limit(encodingLimits, "slice");
                    e.encodingLimits.contrast = parse_encoding_limit(encodingLimits, "contrast");
                    e.encodingLimits.phase = parse_encoding_limit(encodingLimits, "phase");
                    e.encodingLimits.repetition = parse_encoding_limit(encodingLimits, "repetition");
                    e.encodingLimits.set = parse_encoding_limit(encodingLimits, "set");
                    e.encodingLimits.segment = parse_encoding_limit(encodingLimits, "segment");
                    for (size_t k = 0; k < ISMRMRD_USER_INTS; k++) {
                        std::string name = std::string("user_") + ISMRMRD::to_string(k);
                        e.encodingLimits.user[k] = parse_encoding_limit(encodingLimits, name.c_str());
                    }
                }

                pugi::xml_node trajectory = encoding.child("trajectory");
                if (!trajectory) {
                    throw std::runtime_error("trajectory not found in encoding section");
                } else {
                    e.trajectory = parse_trajectory_type(std::string(encoding.child_value("trajectory")));
                }

                pugi::xml_node trajectoryDescription = encoding.child("trajectoryDescription");

                if (trajectoryDescription) {
                    TrajectoryDescription traj;
                    try {
                        traj.identifier = parse_string(trajectoryDescription, "identifier");
                        traj.userParameterLong =
                            parse_user_parameter_long(trajectoryDescription, "userParameterLong");
                        traj.userParameterDouble =
                            parse_user_parameter_double(trajectoryDescription, "userParameterDouble");
                        traj.userParameterString =
                            parse_user_parameter_string(trajectoryDescription, "userParameterString");

                        traj.comment = parse_optional_string(trajectoryDescription, "comment");
                        e.trajectoryDescription = traj;
                    } catch (std::runtime_error &e) {
                        std::cout << "Error parsing trajectory description: " << e.what() << std::endl;
                        throw;
                    }
                }

                pugi::xml_node parallelImaging = encoding.child("parallelImaging");
                if (parallelImaging) {
                    ParallelImaging info;

                    pugi::xml_node accelerationFactor = parallelImaging.child("accelerationFactor");
                    if (!accelerationFactor) {
                        throw std::runtime_error("Unable to accelerationFactor section in parallelImaging");
                    } else {
                        info.accelerationFactor.kspace_encoding_step_1 = static_cast<ISMRMRD::uint16_t>(ISMRMRD::stoi(accelerationFactor.child_value("kspace_encoding_step_1")));
                        info.accelerationFactor.kspace_encoding_step_2 = static_cast<ISMRMRD::uint16_t>(ISMRMRD::stoi(accelerationFactor.child_value("kspace_encoding_step_2")));
                    }

                    info.calibrationMode = parse_optional_string(parallelImaging, "calibrationMode");
                    info.interleavingDimension = parse_optional_string(parallelImaging, "interleavingDimension");

                    pugi::xml_node multiband = parallelImaging.child("multiband");
                    if (multiband) {
                        Multiband mb;
                        mb.deltaKz = parse_float(multiband, "deltaKz");
                        mb.multiband_factor = static_cast<ISMRMRD::uint32_t>(ISMRMRD::stoi(multiband.child_value("multiband_factor")));

                        pugi::xml_node spacing_node = multiband.child("spacing");
                        do {
                            MultibandSpacing mbs;
                            mbs.dZ = parse_vector_float(spacing_node, "dZ");
                            mb.spacing.push_back(mbs);
                            spacing_node = spacing_node.next_sibling("spacing");
                        } while (spacing_node);

                        mb.calibration = parse_multiband_type(multiband.child_value("calibration"));
                        mb.calibration_encoding = ISMRMRD::stoul(multiband.child_value("calibration_encoding"));
                        info.multiband = mb;
                    }
                    e.parallelImaging = info;
                }

                e.echoTrainLength = parse_optional_long(encoding, "echoTrainLength");

                h.encoding.push_back(e);
                encoding = encoding.next_sibling("encoding");
            }
        }

        if (subjectInformation) {
            SubjectInformation info;
            info.patientName = parse_optional_string(subjectInformation, "patientName");
            info.patientWeight_kg = parse_optional_float(subjectInformation, "patientWeight_kg");
            info.patientHeight_m = parse_optional_float(subjectInformation, "patientHeight_m");
            info.patientID = parse_optional_string(subjectInformation, "patientID");
            info.patientBirthdate = parse_optional_string(subjectInformation, "patientBirthdate");
            info.patientGender = parse_optional_string(subjectInformation, "patientGender");
            h.subjectInformation = info;
        }

        if (studyInformation) {
            StudyInformation info;
            info.studyDate = parse_optional_string(studyInformation, "studyDate");
            info.studyTime = parse_optional_string(studyInformation, "studyTime");
            info.studyID = parse_optional_string(studyInformation, "studyID");
            info.accessionNumber = parse_optional_long(studyInformation, "accessionNumber");
            info.referringPhysicianName = parse_optional_string(studyInformation, "referringPhysicianName");
            info.studyDescription = parse_optional_string(studyInformation, "studyDescription");
            info.studyInstanceUID = parse_optional_string(studyInformation, "studyInstanceUID");
            info.bodyPartExamined = parse_optional_string(studyInformation, "bodyPartExamined");
            h.studyInformation = info;
        }

        if (measurementInformation) {
            MeasurementInformation info;
            info.measurementID = parse_optional_string(measurementInformation, "measurementID");
            info.seriesDate = parse_optional_string(measurementInformation, "seriesDate");
            info.seriesTime = parse_optional_string(measurementInformation, "seriesTime");
            info.patientPosition = parse_string(measurementInformation, "patientPosition");
            info.relativeTablePosition = parse_optional_threeDimensionalFloat(measurementInformation, "relativeTablePosition");
            info.initialSeriesNumber = parse_optional_long(measurementInformation, "initialSeriesNumber");
            info.protocolName = parse_optional_string(measurementInformation, "protocolName");
            info.sequenceName = parse_optional_string(measurementInformation, "sequenceName");
            info.seriesDescription = parse_optional_string(measurementInformation, "seriesDescription");
            pugi::xml_node measurementDependency = measurementInformation.child("measurementDependency");
            while (measurementDependency) {
                try {
                    MeasurementDependency d;
                    d.measurementID = parse_string(measurementDependency, "measurementID");
                    d.dependencyType = parse_string(measurementDependency, "dependencyType");
                    info.measurementDependency.push_back(d);
                } catch (std::runtime_error &e) {
                    std::cout << "Error parsing measurement dependency: " << e.what() << std::endl;
                    throw;
                }
                measurementDependency = measurementDependency.next_sibling("measurementDependency");
            }

            info.seriesInstanceUIDRoot = parse_optional_string(measurementInformation, "seriesInstanceUIDRoot");
            info.frameOfReferenceUID = parse_optional_string(measurementInformation, "frameOfReferenceUID");

            // This part of the schema is totally messed up and needs to be fixed, but for now we will just read it.
            pugi::xml_node ri = measurementInformation.child("referencedImageSequence");
            if (ri) {
                pugi::xml_node ric = ri.child("referencedSOPInstanceUID");
                while (ric) {
                    ReferencedImageSequence r;
                    r.referencedSOPInstanceUID = ric.child_value();
                    info.referencedImageSequence.push_back(r);
                    ric = ric.next_sibling("referencedSOPInstanceUID");
                }
            }

            h.measurementInformation = info;
        }

        if (acquisitionSystemInformation) {
            AcquisitionSystemInformation info;
            info.systemVendor = parse_optional_string(acquisitionSystemInformation, "systemVendor");
            info.systemModel = parse_optional_string(acquisitionSystemInformation, "systemModel");
            info.systemFieldStrength_T = parse_optional_float(acquisitionSystemInformation, "systemFieldStrength_T");
            info.relativeReceiverNoiseBandwidth = parse_optional_float(acquisitionSystemInformation, "relativeReceiverNoiseBandwidth");
            info.receiverChannels = parse_optional_ushort(acquisitionSystemInformation, "receiverChannels");
            pugi::xml_node coilLabel = acquisitionSystemInformation.child("coilLabel");
            while (coilLabel) {
                CoilLabel l;
                l.coilNumber = static_cast<uint16_t>(ISMRMRD::stoi(coilLabel.child_value("coilNumber")));
                l.coilName = parse_string(coilLabel, "coilName");
                info.coilLabel.push_back(l);
                coilLabel = coilLabel.next_sibling("coilLabel");
            }
            info.institutionName = parse_optional_string(acquisitionSystemInformation, "institutionName");
            info.stationName = parse_optional_string(acquisitionSystemInformation, "stationName");
            info.deviceID = parse_optional_string(acquisitionSystemInformation, "deviceID");
            info.deviceSerialNumber = parse_optional_string(acquisitionSystemInformation, "deviceSerialNumber");
            h.acquisitionSystemInformation = info;
        }

        if (sequenceParameters) {
            SequenceParameters p;

            std::vector<float> r;
            r = parse_vector_float(sequenceParameters, "TR");
            if (!r.empty())
                p.TR = r;

            r = parse_vector_float(sequenceParameters, "TE");
            if (!r.empty())
                p.TE = r;

            r = parse_vector_float(sequenceParameters, "TI");
            if (!r.empty())
                p.TI = r;

            r = parse_vector_float(sequenceParameters, "flipAngle_deg");
            if (!r.empty())
                p.flipAngle_deg = r;

            std::string diffusiondimension = std::string(sequenceParameters.child_value("diffusionDimension"));
            if (!diffusiondimension.empty())
                p.diffusionDimension = parse_diffusiondimension(diffusiondimension);

            p.diffusion = parse_diffusion_vector(sequenceParameters);

            p.diffusionScheme = parse_optional_string(sequenceParameters, "diffusionScheme");

            p.sequence_type = parse_optional_string(sequenceParameters, "sequence_type");

            r = parse_vector_float(sequenceParameters, "echo_spacing");
            if (!r.empty())
                p.echo_spacing = r;

            h.sequenceParameters = p;
        }

        if (userParameters) {
            UserParameters p;
            p.userParameterLong = parse_user_parameter_long(userParameters, "userParameterLong");
            p.userParameterDouble = parse_user_parameter_double(userParameters, "userParameterDouble");
            p.userParameterString = parse_user_parameter_string(userParameters, "userParameterString");
            p.userParameterBase64 = parse_user_parameter_string(userParameters, "userParameterBase64");
            h.userParameters = p;
        }

        while (waveformInformation) {
            WaveformInformation w;
            w.waveformName = parse_string(waveformInformation, "waveformName");
            w.waveformType = parse_waveform_type(parse_string(waveformInformation, "waveformType"));
            pugi::xml_node waveformUserparameters = waveformInformation.child("userParameters");
            if (waveformUserparameters) {
                UserParameters p;
                p.userParameterLong = parse_user_parameter_long(waveformUserparameters, "userParameterLong");
                p.userParameterDouble = parse_user_parameter_double(waveformUserparameters, "userParameterDouble");
                p.userParameterString = parse_user_parameter_string(waveformUserparameters, "userParameterString");
                p.userParameterBase64 = parse_user_parameter_string(waveformUserparameters, "userParameterBase64");
                w.userParameters = p;
            }
            h.waveformInformation.push_back(w);
            waveformInformation = waveformInformation.next_sibling();
        }
    } else {
        throw std::runtime_error("Root node 'ismrmrdHeader' not found");
    }
}

using ISMRMRD::to_string;
std::string to_string(const std::string &s) { return s; }

std::string to_string(TrajectoryType v) {
    switch (v) {
    case TrajectoryType::CARTESIAN:
        return "cartesian";
    case TrajectoryType::EPI:
        return "epi";
    case TrajectoryType::RADIAL:
        return "radial";
    case TrajectoryType::GOLDENANGLE:
        return "goldenangle";
    case TrajectoryType::SPIRAL:
        return "spiral";
    case TrajectoryType::OTHER:
        return "other";
    }
    throw std::runtime_error("Illegal enum class value");
}

std::string to_string(const WaveformType &v) {
    switch (v) {
    case WaveformType::ECG:
        return "ecg";
    case WaveformType::PULSE:
        return "pulse";
    case WaveformType::RESPIRATORY:
        return "respiratory";
    case WaveformType::TRIGGER:
        return "trigger";
    case WaveformType::GRADIENTWAVEFORM:
        return "gradientwaveform";
    case WaveformType::OTHER:
        return "other";
    }

    throw std::runtime_error("Illegal enum class value");
}

std::string to_string(const DiffusionDimension &d) {
    switch (d) {
    case DiffusionDimension::AVERAGE:
        return "average";
    case DiffusionDimension::CONTRAST:
        return "contrast";
    case DiffusionDimension::PHASE:
        return "phase";
    case DiffusionDimension::REPETITION:
        return "repetition";
    case DiffusionDimension::SET:
        return "set";
    case DiffusionDimension::SEGMENT:
        return "segment";
    case DiffusionDimension::USER_0:
        return "user_0";
    case DiffusionDimension::USER_1:
        return "user_1";
    case DiffusionDimension::USER_2:
        return "user_2";
    case DiffusionDimension::USER_3:
        return "user_3";
    case DiffusionDimension::USER_4:
        return "user_4";
    case DiffusionDimension::USER_5:
        return "user_5";
    case DiffusionDimension::USER_6:
        return "user_6";
    case DiffusionDimension::USER_7:
        return "user_7";
    }
    throw std::runtime_error("Illegal enum class value");
}

std::string to_string(const MultibandCalibrationType &v) {
    switch (v) {
    case MultibandCalibrationType::FULL3D:
        return "full3D";
    case MultibandCalibrationType::SEPARABLE2D:
        return "separable2D";
    case MultibandCalibrationType::OTHER:
        return "other";
    }

    throw std::runtime_error("Illegal enum class value");
}

template <class T>
void append_optional_node(pugi::xml_node &n, const char *child, const Optional<T> &v) {
    if (v) {
        pugi::xml_node n2 = n.append_child(child);
        std::string v_as_string = to_string(*v);
        n2.append_child(pugi::node_pcdata).set_value(v_as_string.c_str());
    }
}

template <class T>
void append_node(pugi::xml_node &n, const char *child, const T &v) {
    pugi::xml_node n2 = n.append_child(child);
    std::string v_as_string = to_string(v);
    n2.append_child(pugi::node_pcdata).set_value(v_as_string.c_str());
}

void append_encoding_space(pugi::xml_node &n, const char *child, const EncodingSpace &s) {
    pugi::xml_node n2 = n.append_child(child);
    pugi::xml_node n3 = n2.append_child("matrixSize");
    append_node(n3, "x", s.matrixSize.x);
    append_node(n3, "y", s.matrixSize.y);
    append_node(n3, "z", s.matrixSize.z);
    n3 = n2.append_child("fieldOfView_mm");
    append_node(n3, "x", s.fieldOfView_mm.x);
    append_node(n3, "y", s.fieldOfView_mm.y);
    append_node(n3, "z", s.fieldOfView_mm.z);
}

void append_optional_three_dimensional_float(pugi::xml_node &n, const char *child, const Optional<threeDimensionalFloat> &s) {
    if (s) {
        pugi::xml_node n2 = n.append_child(child);
        append_node(n2, "x", s->x);
        append_node(n2, "y", s->y);
        append_node(n2, "z", s->z);
    }
}
void append_encoding_limit(pugi::xml_node &n, const char *child, const Optional<Limit> &l) {
    if (l) {
        pugi::xml_node n2 = n.append_child(child);
        append_node(n2, "minimum", l->minimum);
        append_node(n2, "maximum", l->maximum);
        append_node(n2, "center", l->center);
    }
}

template <class T>
void append_user_parameter(pugi::xml_node &n, const char *child,
                           const std::vector<T> &v) {
    for (size_t i = 0; i < v.size(); i++) {
        pugi::xml_node n2 = n.append_child(child);
        append_node(n2, "name", v[i].name);
        append_node(n2, "value", v[i].value);
    }
}

void append_waveform_information(pugi::xml_node &n, const char *child, const WaveformInformation &w) {
    pugi::xml_node n2 = n.append_child(child);
    append_node(n2, "waveformName", w.waveformName);
    append_node(n2, "waveformType", w.waveformType);
    if (w.userParameters) {
        pugi::xml_node n3 = n2.append_child("userParameters");
        append_user_parameter(n3, "userParameterLong", w.userParameters->userParameterLong);
        append_user_parameter(n3, "userParameterDouble", w.userParameters->userParameterDouble);
        append_user_parameter(n3, "userParameterString", w.userParameters->userParameterString);
        append_user_parameter(n3, "userParameterBase64", w.userParameters->userParameterBase64);
    }
}

// End utility functions for serialization

void serialize(const IsmrmrdHeader &h, std::ostream &o) {
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child();
    pugi::xml_node n1, n2, n3;
    pugi::xml_attribute a;

    root.set_name("ismrmrdHeader");

    a = root.append_attribute("xmlns");
    a.set_value("http://www.ismrm.org/ISMRMRD");

    a = root.append_attribute("xmlns:xsi");
    a.set_value("http://www.w3.org/2001/XMLSchema-instance");

    a = root.append_attribute("xmlns:xs");
    a.set_value("http://www.w3.org/2001/XMLSchema");

    a = root.append_attribute("xsi:schemaLocation");
    a.set_value("http://www.ismrm.org/ISMRMRD ismrmrd.xsd");

    if (h.version) {
        if (*h.version != ISMRMRD_XMLHDR_VERSION) {
            throw std::runtime_error("XML header version does not match library schema version.");
        }
        append_optional_node(root, "version", h.version);
    }

    if (h.subjectInformation) {
        n1 = root.append_child();
        n1.set_name("subjectInformation");
        append_optional_node(n1, "patientName", h.subjectInformation->patientName);
        append_optional_node(n1, "patientWeight_kg", h.subjectInformation->patientWeight_kg);
        append_optional_node(n1, "patientHeight_m", h.subjectInformation->patientHeight_m);
        append_optional_node(n1, "patientID", h.subjectInformation->patientID);
        append_optional_node(n1, "patientBirthdate", h.subjectInformation->patientBirthdate);
        append_optional_node(n1, "patientGender", h.subjectInformation->patientGender);
    }

    if (h.studyInformation) {
        n1 = root.append_child();
        n1.set_name("studyInformation");
        append_optional_node(n1, "studyDate", h.studyInformation->studyDate);
        append_optional_node(n1, "studyTime", h.studyInformation->studyTime);
        append_optional_node(n1, "studyID", h.studyInformation->studyID);
        append_optional_node(n1, "accessionNumber", h.studyInformation->accessionNumber);
        append_optional_node(n1, "referringPhysicianName", h.studyInformation->referringPhysicianName);
        append_optional_node(n1, "studyDescription", h.studyInformation->studyDescription);
        append_optional_node(n1, "studyInstanceUID", h.studyInformation->studyInstanceUID);
        append_optional_node(n1, "bodyPartExamined", h.studyInformation->bodyPartExamined);
    }

    if (h.measurementInformation) {
        n1 = root.append_child();
        n1.set_name("measurementInformation");
        append_optional_node(n1, "measurementID", h.measurementInformation->measurementID);
        append_optional_node(n1, "seriesDate", h.measurementInformation->seriesDate);
        append_optional_node(n1, "seriesTime", h.measurementInformation->seriesTime);
        append_node(n1, "patientPosition", h.measurementInformation->patientPosition);
        append_optional_three_dimensional_float(n1, "relativeTablePosition", h.measurementInformation->relativeTablePosition);
        append_optional_node(n1, "initialSeriesNumber", h.measurementInformation->initialSeriesNumber);
        append_optional_node(n1, "protocolName", h.measurementInformation->protocolName);
        append_optional_node(n1, "sequenceName", h.measurementInformation->sequenceName);
        append_optional_node(n1, "seriesDescription", h.measurementInformation->seriesDescription);

        for (size_t i = 0; i < h.measurementInformation->measurementDependency.size(); i++) {
            n2 = n1.append_child();
            n2.set_name("measurementDependency");
            append_node(n2, "dependencyType", h.measurementInformation->measurementDependency[i].dependencyType);
            append_node(n2, "measurementID", h.measurementInformation->measurementDependency[i].measurementID);
        }

        append_optional_node(n1, "seriesInstanceUIDRoot", h.measurementInformation->seriesInstanceUIDRoot);
        append_optional_node(n1, "frameOfReferenceUID", h.measurementInformation->frameOfReferenceUID);

        // TODO: Sort out stuff with this referenced image sequence. This is all messed up.
        if (h.measurementInformation->referencedImageSequence.size()) {
            n2 = n1.append_child("referencedImageSequence");
            for (size_t i = 0; i < h.measurementInformation->referencedImageSequence.size(); i++) {
                append_node(n2, "referencedSOPInstanceUID", h.measurementInformation->referencedImageSequence[i].referencedSOPInstanceUID);
            }
        }
    }

    if (h.acquisitionSystemInformation) {
        n1 = root.append_child();
        n1.set_name("acquisitionSystemInformation");
        append_optional_node(n1, "systemVendor", h.acquisitionSystemInformation->systemVendor);
        append_optional_node(n1, "systemModel", h.acquisitionSystemInformation->systemModel);
        append_optional_node(n1, "systemFieldStrength_T", h.acquisitionSystemInformation->systemFieldStrength_T);
        append_optional_node(n1, "relativeReceiverNoiseBandwidth", h.acquisitionSystemInformation->relativeReceiverNoiseBandwidth);
        append_optional_node(n1, "receiverChannels", h.acquisitionSystemInformation->receiverChannels);
        for (size_t i = 0; i < h.acquisitionSystemInformation->coilLabel.size(); i++) {
            n2 = n1.append_child();
            n2.set_name("coilLabel");
            append_node(n2, "coilNumber", h.acquisitionSystemInformation->coilLabel[i].coilNumber);
            append_node(n2, "coilName", h.acquisitionSystemInformation->coilLabel[i].coilName);
        }
        append_optional_node(n1, "institutionName", h.acquisitionSystemInformation->institutionName);
        append_optional_node(n1, "stationName", h.acquisitionSystemInformation->stationName);
        append_optional_node(n1, "deviceID", h.acquisitionSystemInformation->deviceID);
        append_optional_node(n1, "deviceSerialNumber", h.acquisitionSystemInformation->deviceSerialNumber);
    }

    n1 = root.append_child();
    n1.set_name("experimentalConditions");
    append_node(n1, "H1resonanceFrequency_Hz", h.experimentalConditions.H1resonanceFrequency_Hz);

    if (!h.encoding.size()) {
        throw std::runtime_error("Encoding array is empty. Invalid ISMRMRD header structure");
    }

    for (size_t i = 0; i < h.encoding.size(); i++) {
        n1 = root.append_child("encoding");
        append_encoding_space(n1, "encodedSpace", h.encoding[i].encodedSpace);
        append_encoding_space(n1, "reconSpace", h.encoding[i].reconSpace);
        n2 = n1.append_child("encodingLimits");
        append_encoding_limit(n2, "kspace_encoding_step_0", h.encoding[i].encodingLimits.kspace_encoding_step_0);
        append_encoding_limit(n2, "kspace_encoding_step_1", h.encoding[i].encodingLimits.kspace_encoding_step_1);
        append_encoding_limit(n2, "kspace_encoding_step_2", h.encoding[i].encodingLimits.kspace_encoding_step_2);
        append_encoding_limit(n2, "average", h.encoding[i].encodingLimits.average);
        append_encoding_limit(n2, "slice", h.encoding[i].encodingLimits.slice);
        append_encoding_limit(n2, "contrast", h.encoding[i].encodingLimits.contrast);
        append_encoding_limit(n2, "phase", h.encoding[i].encodingLimits.phase);
        append_encoding_limit(n2, "repetition", h.encoding[i].encodingLimits.repetition);
        append_encoding_limit(n2, "set", h.encoding[i].encodingLimits.set);
        append_encoding_limit(n2, "segment", h.encoding[i].encodingLimits.segment);

        for (size_t k = 0; k < ISMRMRD_USER_INTS; k++) {
            std::string name = std::string("user_") + ISMRMRD::to_string(k);
            append_encoding_limit(n2, name.c_str(), h.encoding[i].encodingLimits.user[k]);
        }

        append_node(n1, "trajectory", h.encoding[i].trajectory);

        if (h.encoding[i].trajectoryDescription) {
            n2 = n1.append_child("trajectoryDescription");
            append_node(n2, "identifier", h.encoding[i].trajectoryDescription->identifier);
            append_user_parameter(n2, "userParameterLong", h.encoding[i].trajectoryDescription->userParameterLong);
            append_user_parameter(n2, "userParameterDouble", h.encoding[i].trajectoryDescription->userParameterDouble);
            append_user_parameter(n2, "userParameterString", h.encoding[i].trajectoryDescription->userParameterString);
            append_optional_node(n2, "comment", h.encoding[i].trajectoryDescription->comment);
        }

        if (h.encoding[i].parallelImaging) {
            const ParallelImaging &parallelImaging = *h.encoding[i].parallelImaging;
            n2 = n1.append_child("parallelImaging");
            n3 = n2.append_child("accelerationFactor");
            append_node(n3, "kspace_encoding_step_1", parallelImaging.accelerationFactor.kspace_encoding_step_1);
            append_node(n3, "kspace_encoding_step_2", parallelImaging.accelerationFactor.kspace_encoding_step_2);
            append_optional_node(n2, "calibrationMode", parallelImaging.calibrationMode);
            append_optional_node(n2, "interleavingDimension", parallelImaging.interleavingDimension);

            if (parallelImaging.multiband) {
                const Multiband &multiband = *parallelImaging.multiband;
                pugi::xml_node n4 = n2.append_child("multiband");
                //        for (const auto& mb : multiband.spacing){
                for (std::vector<MultibandSpacing>::const_iterator mb = multiband.spacing.begin(); mb != multiband.spacing.end(); ++mb) {
                    pugi::xml_node n5 = n4.append_child("spacing");
                    //            for (const auto dZ : mb.dZ){
                    for (std::vector<float>::const_iterator dZ = mb->dZ.begin(); dZ != mb->dZ.end(); ++dZ) {
                        append_node(n5, "dZ", *dZ);
                    }
                }
                append_node(n4, "deltaKz", multiband.deltaKz);
                append_node(n4, "multiband_factor", multiband.multiband_factor);
                append_node(n4, "calibration", multiband.calibration);
                append_node(n4, "calibration_encoding", multiband.calibration_encoding);
            }
        }

        append_optional_node(n1, "echoTrainLength", h.encoding[i].echoTrainLength);
    }

    if (h.sequenceParameters) {
        n1 = root.append_child("sequenceParameters");

        if (h.sequenceParameters->TR.is_present()) {
            for (size_t i = 0; i < h.sequenceParameters->TR->size(); i++) {
                append_node(n1, "TR", h.sequenceParameters->TR->operator[](i));
            }
        }

        if (h.sequenceParameters->TE.is_present()) {
            for (size_t i = 0; i < h.sequenceParameters->TE->size(); i++) {
                append_node(n1, "TE", h.sequenceParameters->TE->operator[](i));
            }
        }

        if (h.sequenceParameters->TI.is_present()) {
            for (size_t i = 0; i < h.sequenceParameters->TI->size(); i++) {
                append_node(n1, "TI", h.sequenceParameters->TI->operator[](i));
            }
        }

        if (h.sequenceParameters->flipAngle_deg.is_present()) {
            for (size_t i = 0; i < h.sequenceParameters->flipAngle_deg->size(); i++) {
                append_node(n1, "flipAngle_deg", h.sequenceParameters->flipAngle_deg->operator[](i));
            }
        }

        append_optional_node(n1, "sequence_type", h.sequenceParameters->sequence_type);

        if (h.sequenceParameters->echo_spacing.is_present()) {
            for (size_t i = 0; i < h.sequenceParameters->echo_spacing->size(); i++) {
                append_node(n1, "echo_spacing", h.sequenceParameters->echo_spacing->operator[](i));
            }
        }

        append_optional_node(n1, "diffusionDimension", h.sequenceParameters->diffusionDimension);
        if (h.sequenceParameters->diffusion) {

            // for (const auto& diff : h.sequenceParameters->diffusion.get()){
            for (std::vector<Diffusion>::const_iterator diff = h.sequenceParameters->diffusion->begin(); diff != h.sequenceParameters->diffusion->end(); ++diff) {
                pugi::xml_node diff_node = n1.append_child("diffusion");
                append_node(diff_node, "bvalue", diff->bvalue);
                pugi::xml_node grad_node = diff_node.append_child("gradientDirection");
                append_node(grad_node, "rl", diff->gradientDirection.rl);
                append_node(grad_node, "ap", diff->gradientDirection.ap);
                append_node(grad_node, "fh", diff->gradientDirection.fh);
            }
        }
        append_optional_node(n1, "diffusionScheme", h.sequenceParameters->diffusionScheme);
    }

    if (h.userParameters) {
        n1 = root.append_child("userParameters");
        append_user_parameter(n1, "userParameterLong", h.userParameters->userParameterLong);
        append_user_parameter(n1, "userParameterDouble", h.userParameters->userParameterDouble);
        append_user_parameter(n1, "userParameterString", h.userParameters->userParameterString);
        append_user_parameter(n1, "userParameterBase64", h.userParameters->userParameterBase64);
    }

    // for (auto w : h.waveformInformation){
    for (std::vector<WaveformInformation>::const_iterator w = h.waveformInformation.begin(); w != h.waveformInformation.end(); ++w) {
        append_waveform_information(root, "waveformInformation", *w);
    }

    doc.save(o);
}

std::ostream &operator<<(std::ostream &stream, const IsmrmrdHeader &header) {
    serialize(header, stream);
    return stream;
}

bool operator==(const IsmrmrdHeader &lhs, const IsmrmrdHeader &rhs) {
    return lhs.version == rhs.version &&
           lhs.subjectInformation == rhs.subjectInformation &&
           lhs.studyInformation == rhs.studyInformation &&
           lhs.measurementInformation == rhs.measurementInformation &&
           lhs.acquisitionSystemInformation == rhs.acquisitionSystemInformation &&
           lhs.experimentalConditions == rhs.experimentalConditions &&
           lhs.encoding == rhs.encoding &&
           lhs.sequenceParameters == rhs.sequenceParameters &&
           lhs.userParameters == rhs.userParameters &&
           lhs.waveformInformation == rhs.waveformInformation;
}
bool operator!=(const IsmrmrdHeader &lhs, const IsmrmrdHeader &rhs) {
    return !(rhs == lhs);
}
bool operator==(const SubjectInformation &lhs, const SubjectInformation &rhs) {
    return lhs.patientName == rhs.patientName &&
           lhs.patientWeight_kg == rhs.patientWeight_kg &&
           lhs.patientID == rhs.patientID &&
           lhs.patientBirthdate == rhs.patientBirthdate &&
           lhs.patientGender == rhs.patientGender;
}
bool operator!=(const SubjectInformation &lhs, const SubjectInformation &rhs) {
    return !(rhs == lhs);
}
bool operator==(const StudyInformation &lhs, const StudyInformation &rhs) {
    return lhs.studyDate == rhs.studyDate &&
           lhs.studyTime == rhs.studyTime &&
           lhs.studyID == rhs.studyID &&
           lhs.accessionNumber == rhs.accessionNumber &&
           lhs.referringPhysicianName == rhs.referringPhysicianName &&
           lhs.studyDescription == rhs.studyDescription &&
           lhs.studyInstanceUID == rhs.studyInstanceUID &&
           lhs.bodyPartExamined == rhs.bodyPartExamined;
}
bool operator!=(const StudyInformation &lhs, const StudyInformation &rhs) {
    return !(rhs == lhs);
}
bool operator<(const MeasurementDependency &lhs, const MeasurementDependency &rhs) {
    return lhs.dependencyType < rhs.dependencyType || (lhs.dependencyType == rhs.dependencyType && lhs.measurementID < rhs.measurementID);
}
bool operator>(const MeasurementDependency &lhs, const MeasurementDependency &rhs) {
    return rhs < lhs;
}
bool operator<=(const MeasurementDependency &lhs, const MeasurementDependency &rhs) {
    return !(rhs < lhs);
}
bool operator>=(const MeasurementDependency &lhs, const MeasurementDependency &rhs) {
    return !(lhs < rhs);
}
bool operator==(const ReferencedImageSequence &lhs, const ReferencedImageSequence &rhs) {
    return lhs.referencedSOPInstanceUID == rhs.referencedSOPInstanceUID;
}
bool operator!=(const ReferencedImageSequence &lhs, const ReferencedImageSequence &rhs) {
    return !(rhs == lhs);
}
bool operator==(const MeasurementInformation &lhs, const MeasurementInformation &rhs) {
    return lhs.measurementID == rhs.measurementID && lhs.seriesDate == rhs.seriesDate && lhs.seriesTime == rhs.seriesTime && lhs.patientPosition == rhs.patientPosition && lhs.relativeTablePosition == rhs.relativeTablePosition && lhs.initialSeriesNumber == rhs.initialSeriesNumber && lhs.protocolName == rhs.protocolName && lhs.seriesDescription == rhs.seriesDescription && lhs.measurementDependency == rhs.measurementDependency && lhs.seriesInstanceUIDRoot == rhs.seriesInstanceUIDRoot && lhs.frameOfReferenceUID == rhs.frameOfReferenceUID && lhs.referencedImageSequence == rhs.referencedImageSequence;
}
bool operator!=(const MeasurementInformation &lhs, const MeasurementInformation &rhs) {
    return !(rhs == lhs);
}
bool operator==(const CoilLabel &lhs, const CoilLabel &rhs) {
    return lhs.coilNumber == rhs.coilNumber && lhs.coilName == rhs.coilName;
}
bool operator!=(const CoilLabel &lhs, const CoilLabel &rhs) {
    return !(rhs == lhs);
}
bool operator==(const AcquisitionSystemInformation &lhs, const AcquisitionSystemInformation &rhs) {
    return lhs.systemVendor == rhs.systemVendor && lhs.systemModel == rhs.systemModel && lhs.systemFieldStrength_T == rhs.systemFieldStrength_T && lhs.relativeReceiverNoiseBandwidth == rhs.relativeReceiverNoiseBandwidth && lhs.receiverChannels == rhs.receiverChannels && lhs.coilLabel == rhs.coilLabel && lhs.institutionName == rhs.institutionName && lhs.stationName == rhs.stationName && lhs.deviceID == rhs.deviceID && lhs.deviceSerialNumber == rhs.deviceSerialNumber;
}
bool operator!=(const AcquisitionSystemInformation &lhs, const AcquisitionSystemInformation &rhs) {
    return !(rhs == lhs);
}
bool operator==(const ExperimentalConditions &lhs, const ExperimentalConditions &rhs) {
    return lhs.H1resonanceFrequency_Hz == rhs.H1resonanceFrequency_Hz;
}
bool operator!=(const ExperimentalConditions &lhs, const ExperimentalConditions &rhs) {
    return !(rhs == lhs);
}
bool operator==(const MatrixSize &lhs, const MatrixSize &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
bool operator!=(const MatrixSize &lhs, const MatrixSize &rhs) {
    return !(rhs == lhs);
}
bool operator==(const FieldOfView_mm &lhs, const FieldOfView_mm &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
bool operator!=(const FieldOfView_mm &lhs, const FieldOfView_mm &rhs) {
    return !(rhs == lhs);
}
bool operator==(const EncodingSpace &lhs, const EncodingSpace &rhs) {
    return lhs.matrixSize == rhs.matrixSize && lhs.fieldOfView_mm == rhs.fieldOfView_mm;
}
bool operator!=(const EncodingSpace &lhs, const EncodingSpace &rhs) {
    return !(rhs == lhs);
}
bool operator==(const Limit &lhs, const Limit &rhs) {
    return lhs.minimum == rhs.minimum && lhs.maximum == rhs.maximum && lhs.center == rhs.center;
}
bool operator!=(const Limit &lhs, const Limit &rhs) {
    return !(rhs == lhs);
}
bool operator==(const EncodingLimits &lhs, const EncodingLimits &rhs) {
    return lhs.kspace_encoding_step_0 == rhs.kspace_encoding_step_0 && lhs.kspace_encoding_step_1 == rhs.kspace_encoding_step_1 && lhs.kspace_encoding_step_2 == rhs.kspace_encoding_step_2 && lhs.average == rhs.average && lhs.slice == rhs.slice && lhs.contrast == rhs.contrast && lhs.phase == rhs.phase && lhs.repetition == rhs.repetition && lhs.set == rhs.set && lhs.segment == rhs.segment;
}
bool operator!=(const EncodingLimits &lhs, const EncodingLimits &rhs) {
    return !(rhs == lhs);
}
bool operator==(const UserParameterLong &lhs, const UserParameterLong &rhs) {
    return lhs.name == rhs.name && lhs.value == rhs.value;
}
bool operator!=(const UserParameterLong &lhs, const UserParameterLong &rhs) {
    return !(rhs == lhs);
}
bool operator==(const UserParameterDouble &lhs, const UserParameterDouble &rhs) {
    return lhs.name == rhs.name && lhs.value == rhs.value;
}
bool operator!=(const UserParameterDouble &lhs, const UserParameterDouble &rhs) {
    return !(rhs == lhs);
}
bool operator==(const UserParameterString &lhs, const UserParameterString &rhs) {
    return lhs.name == rhs.name && lhs.value == rhs.value;
}
bool operator!=(const UserParameterString &lhs, const UserParameterString &rhs) {
    return !(rhs == lhs);
}
bool operator==(const UserParameters &lhs, const UserParameters &rhs) {
    return lhs.userParameterLong == rhs.userParameterLong && lhs.userParameterDouble == rhs.userParameterDouble && lhs.userParameterString == rhs.userParameterString && lhs.userParameterBase64 == rhs.userParameterBase64;
}
bool operator!=(const UserParameters &lhs, const UserParameters &rhs) {
    return !(rhs == lhs);
}
bool operator==(const TrajectoryDescription &lhs, const TrajectoryDescription &rhs) {
    return lhs.identifier == rhs.identifier && lhs.userParameterLong == rhs.userParameterLong && lhs.userParameterDouble == rhs.userParameterDouble && lhs.comment == rhs.comment;
}
bool operator!=(const TrajectoryDescription &lhs, const TrajectoryDescription &rhs) {
    return !(rhs == lhs);
}
bool operator==(const AccelerationFactor &lhs, const AccelerationFactor &rhs) {
    return lhs.kspace_encoding_step_1 == rhs.kspace_encoding_step_1 && lhs.kspace_encoding_step_2 == rhs.kspace_encoding_step_2;
}
bool operator!=(const AccelerationFactor &lhs, const AccelerationFactor &rhs) {
    return !(rhs == lhs);
}
bool operator==(const ParallelImaging &lhs, const ParallelImaging &rhs) {
    return lhs.accelerationFactor == rhs.accelerationFactor && lhs.calibrationMode == rhs.calibrationMode && lhs.interleavingDimension == rhs.interleavingDimension && lhs.multiband == rhs.multiband;
}
bool operator!=(const ParallelImaging &lhs, const ParallelImaging &rhs) {
    return !(rhs == lhs);
}
bool operator==(const Multiband &lhs, const Multiband &rhs) {
    return lhs.spacing == rhs.spacing && lhs.deltaKz == rhs.deltaKz && lhs.multiband_factor == rhs.multiband_factor && lhs.calibration == rhs.calibration && lhs.calibration_encoding == rhs.calibration_encoding;
}
bool operator!=(const Multiband &lhs, const Multiband &rhs) {
    return !(rhs == lhs);
}
bool operator==(const Encoding &lhs, const Encoding &rhs) {
    return lhs.encodedSpace == rhs.encodedSpace && lhs.reconSpace == rhs.reconSpace && lhs.encodingLimits == rhs.encodingLimits && lhs.trajectory == rhs.trajectory && lhs.trajectoryDescription == rhs.trajectoryDescription && lhs.parallelImaging == rhs.parallelImaging && lhs.echoTrainLength == rhs.echoTrainLength;
}
bool operator!=(const Encoding &lhs, const Encoding &rhs) {
    return !(rhs == lhs);
}
bool operator==(const SequenceParameters &lhs, const SequenceParameters &rhs) {
    return lhs.TR == rhs.TR && lhs.TE == rhs.TE && lhs.TI == rhs.TI && lhs.flipAngle_deg == rhs.flipAngle_deg && lhs.sequence_type == rhs.sequence_type && lhs.echo_spacing == rhs.echo_spacing && lhs.diffusion == rhs.diffusion && lhs.diffusionDimension == rhs.diffusionDimension && lhs.diffusionScheme == rhs.diffusionScheme;
}
bool operator!=(const SequenceParameters &lhs, const SequenceParameters &rhs) {
    return !(rhs == lhs);
}
bool operator==(const WaveformInformation &lhs, const WaveformInformation &rhs) {
    return lhs.waveformName == rhs.waveformName && lhs.waveformType == rhs.waveformType && lhs.userParameters == rhs.userParameters;
}
bool operator!=(const WaveformInformation &lhs, const WaveformInformation &rhs) {
    return !(rhs == lhs);
}
bool operator==(const threeDimensionalFloat &lhs, const threeDimensionalFloat &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
bool operator!=(const threeDimensionalFloat &lhs, const threeDimensionalFloat &rhs) {
    return !(rhs == lhs);
}
bool operator==(const MeasurementDependency &lhs, const MeasurementDependency &rhs) {
    return lhs.dependencyType == rhs.dependencyType && lhs.measurementID == rhs.measurementID;
}
bool operator!=(const MeasurementDependency &lhs, const MeasurementDependency &rhs) {
    return !(rhs == lhs);
}
bool operator==(const MultibandSpacing &lhs, const MultibandSpacing &rhs) {
    return lhs.dZ == rhs.dZ;
}
bool operator!=(const MultibandSpacing &lhs, const MultibandSpacing &rhs) {
    return !(rhs == lhs);
}

bool operator==(const Diffusion &lhs, const Diffusion &rhs) {
    return lhs.bvalue == rhs.bvalue && lhs.gradientDirection == rhs.gradientDirection;
}
bool operator!=(const Diffusion &lhs, const Diffusion &rhs) {
    return !(rhs == lhs);
}
bool operator==(const GradientDirection &lhs, const GradientDirection &rhs) {
    return lhs.rl == rhs.rl && lhs.ap == rhs.ap && lhs.fh == rhs.fh;
}
bool operator!=(const GradientDirection &lhs, const GradientDirection &rhs) {
    return !(rhs == lhs);
}
} // namespace ISMRMRD
