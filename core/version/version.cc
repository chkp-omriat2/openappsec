// Copyright (C) 2022 Check Point Software Technologies Ltd. All rights reserved.

// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "version.h"
#include "version_vars.h"
#include "singleton.h"
#include "rest.h"

#include <string>
#include <sstream>

using namespace std;

// All variables (is_public, id, user, timestamp, version_prefix) are defined in version_vars.h.
// version_vars.h generated by build_version_vars_h.py at compilation

class VersionRest : public ServerRest
{
public:
    void
    doCall() override
    {
        timestamp = Version::getTimestamp();
        if (Version::isPublic()) {
            type = "public";
            version = Version::getFullVersion();
        } else {
            type = "private";
            commit = Version::getID();
            user = Version::getUser();
        }
    }

private:
    S2C_PARAM(string, type);
    S2C_PARAM(string, timestamp);
    S2C_OPTIONAL_PARAM(string, version);
    S2C_OPTIONAL_PARAM(string, user);
    S2C_OPTIONAL_PARAM(string, commit);
};

void
Version::init()
{
    Singleton::Consume<I_RestApi>::by<Version>()->addRestCall<VersionRest>(RestAction::SHOW, "version-info");
    Singleton::Consume<I_Environment>::by<Version>()->registerValue<string>("Service Version", getFullVersion());
}

bool
Version::isPublic()
{
    return is_public;
}

string
Version::getID()
{
    return id;
}

string
Version::getFullVersion()
{
    static string version =
        (isPublic() ? getVerPrefix() : "") +
        getID() +
        (isPublic() && getBranch() != "master" ? ("-" + getBranch()) : "");
    return version;
}

string
Version::getUser()
{
    return user;
}

string
Version::getTimestamp()
{
    return timestamp;
}

string
Version::getVerPrefix()
{
    return version_prefix;
}

string
Version::getBranch()
{
    return version_branch;
}

string
Version::get()
{
    ostringstream version_stream;
    if (isPublic()) {
        version_stream
            << "Type: Public, Version: "
            << getFullVersion()
            << ", Created at: "
            << getTimestamp();
    } else {
        version_stream
            << "Type: Private, Git ID: "
            << getFullVersion()
            << ", Created at: "
            << getTimestamp()
            << ", Created by: "
            << getUser();
    }
    return version_stream.str();
}
