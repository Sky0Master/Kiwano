// Copyright (c) 2016-2018 Kiwano - Nomango
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once
#include <kiwano/core/Resource.h>

namespace kiwano
{
/**
 * \~chinese
 * @brief �ļ�ϵͳ��Ϊ����ģ���ṩ�ļ����������
 */
class KGE_API FileSystem : public Singleton<FileSystem>
{
    friend Singleton<FileSystem>;

public:
    /**
     * \~chinese
     * @brief �����ļ�����·��
     * @param path �ļ�����·��
     */
    void AddSearchPath(String const& path);

    /**
     * \~chinese
     * @brief �����ļ�����·��
     * @param paths ����·������
     */
    void SetSearchPaths(Vector<String> const& paths);

    /**
     * \~chinese
     * @brief �������б��в����ļ�����ȡ����·��
     * @param file �ļ�·��
     * @return �������ļ�·��
     */
    String GetFullPathForFile(String const& file) const;

    /**
     * \~chinese
     * @brief �����ļ�·�������ֵ����
     * @param key �ļ��ؼ���
     * @param file_path �ļ�·��
     */
    void AddFileLookupRule(String const& key, String const& file_path);

    /**
     * \~chinese
     * @brief �����ļ�·�������ֵ�
     * @param dict �ļ�·�������ֵ�
     */
    void SetFileLookupDictionary(UnorderedMap<String, String> const& dict);

    /**
     * \~chinese
     * @brief �ļ��Ƿ����
     * @param file_path �ļ�·��
     * @return ���ļ����ڣ����� true
     */
    bool IsFileExists(String const& file_path) const;

    /**
     * \~chinese
     * @brief �ж�·���Ƿ��Ǿ���·��
     * @param path �ļ�·��
     * @return ���Ǿ���·�������� true
     */
    bool IsAbsolutePath(String const& path) const;

    /**
     * \~chinese
     * @brief ɾ���ļ�
     * @param file_path �ļ�·��
     * @return ɾ���Ƿ�ɹ�
     */
    bool RemoveFile(String const& file_path) const;

    /**
     * \~chinese
     * @brief �ͷŶ�������Դ����ʱ�ļ�Ŀ¼
     * @param res ��Դ
     * @param dest_file_name Ŀ���ļ���
     * @return �����Ƿ�ɹ�
     */
    bool ExtractResourceToFile(Resource const& res, String const& dest_file_name) const;

private:
    FileSystem();

    ~FileSystem();

private:
    Vector<String>                       search_paths_;
    UnorderedMap<String, String>         file_lookup_dict_;
    mutable UnorderedMap<String, String> file_lookup_cache_;
};
}  // namespace kiwano