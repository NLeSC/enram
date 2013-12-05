# Copyright 2013 Netherlands eScience Center
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

exec 33<>/dev/tcp/localhost/33334
function _________DEBUG_TRAP ()
{
	local _________DEBUG_COMMAND
	read -u 33 _________DEBUG_COMMAND
	eval $_________DEBUG_COMMAND
}
set -o functrace
trap _________DEBUG_TRAP DEBUG

