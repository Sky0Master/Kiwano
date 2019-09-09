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

#include "ResourceCache.h"
#include "../base/Logger.h"
#include "../2d/Frame.h"
#include "../2d/FrameSequence.h"
#include "../renderer/GifImage.h"
#include "../renderer/FontCollection.h"
#include <fstream>

namespace kiwano
{
	namespace __resource_cache_01
	{
		bool LoadJsonData(ResourceCache* loader, Json const& json_data);
		bool LoadXmlData(ResourceCache* loader, const tinyxml2::XMLElement* elem);
	}

	namespace
	{
		Map<String, Function<bool(ResourceCache*, Json const&)>> load_json_funcs = {
			{ L"latest", __resource_cache_01::LoadJsonData },
			{ L"0.1", __resource_cache_01::LoadJsonData },
		};

		Map<String, Function<bool(ResourceCache*, const tinyxml2::XMLElement*)>> load_xml_funcs = {
			{ L"latest", __resource_cache_01::LoadXmlData },
			{ L"0.1", __resource_cache_01::LoadXmlData },
		};
	}

	ResourceCache::ResourceCache()
	{
	}

	ResourceCache::~ResourceCache()
	{
		Clear();
	}

	bool ResourceCache::LoadFromJsonFile(String const& file_path)
	{
		Json json_data;
		std::wifstream ifs;
		ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			ifs.open(file_path.c_str());
			ifs >> json_data;
			ifs.close();
		}
		catch (std::wifstream::failure& e)
		{
			KGE_WARNING_LOG(L"ResourceCache::LoadFromJsonFile failed: Cannot open file. (%s)", string_to_wide(e.what()).c_str());
			return false;
		}
		catch (json_exception& e)
		{
			KGE_WARNING_LOG(L"ResourceCache::LoadFromJsonFile failed: Cannot parse to JSON. (%s)", string_to_wide(e.what()).c_str());
			return false;
		}
		return LoadFromJson(json_data);
	}

	bool ResourceCache::LoadFromJson(Json const& json_data)
	{
		try
		{
			String version = json_data[L"version"];

			auto load = load_json_funcs.find(version);
			if (load != load_json_funcs.end())
			{
				return load->second(this, json_data);
			}
			else if (version.empty())
			{
				return load_json_funcs[L"latest"](this, json_data);
			}
			else
			{
				throw std::runtime_error("unknown JSON data version");
			}
		}
		catch (std::exception& e)
		{
			KGE_WARNING_LOG(L"ResourceCache::LoadFromJson failed: JSON data is invalid. (%s)", string_to_wide(e.what()).c_str());
			return false;
		}
		return false;
	}

	bool ResourceCache::LoadFromXmlFile(String const& file_path)
	{
		tinyxml2::XMLDocument doc;

		std::wifstream ifs;
		ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			ifs.open(file_path.c_str());

			StringStream ss;
			ss << ifs.rdbuf();

			if (tinyxml2::XML_SUCCESS != doc.Parse(ss.str().c_str()))
			{
				KGE_WARNING_LOG(L"ResourceCache::LoadFromXmlFile failed: %s (%s)",
					tinyxml2::XMLDocument::ErrorIDToName(doc.ErrorID()), doc.ErrorStr());
				return false;
			}
		}
		catch (std::wifstream::failure& e)
		{
			KGE_WARNING_LOG(L"ResourceCache::LoadFromXmlFile failed: Cannot open file. (%s)", string_to_wide(e.what()).c_str());
			return false;
		}

		return LoadFromXml(&doc);
	}

	bool ResourceCache::LoadFromXml(const tinyxml2::XMLDocument* doc)
	{
		if (doc)
		{
			try
			{
				if (auto root = doc->FirstChildElement(L"resources"))
				{
					kiwano::wstring version;
					if (auto ver = root->FirstChildElement(L"version")) version = ver->GetText();

					auto load = load_xml_funcs.find(version);
					if (load != load_xml_funcs.end())
					{
						return load->second(this, root);
					}
					else if (version.empty())
					{
						return load_xml_funcs[L"latest"](this, root);
					}
					else
					{
						throw std::runtime_error("unknown JSON data version");
					}
				}
			}
			catch (std::exception& e)
			{
				KGE_WARNING_LOG(L"ResourceCache::LoadFromXml failed: %s", string_to_wide(e.what()).c_str());
				return false;
			}
		}
		return false;
	}

	bool ResourceCache::AddFrame(String const& id, String const& file_path)
	{
		FramePtr ptr = new (std::nothrow) Frame;
		if (ptr)
		{
			if (ptr->Load(file_path))
			{
				return AddFrame(id, ptr);
			}
		}
		return false;
	}

	bool ResourceCache::AddFrame(String const & id, FramePtr frame)
	{
		if (frame)
		{
			cache_.insert(std::make_pair(id, frame));
			return true;
		}
		return false;
	}

	UInt32 ResourceCache::AddFrameSequence(String const& id, Vector<String> const& files)
	{
		if (files.empty())
			return 0;

		Vector<FramePtr> frames;
		frames.reserve(files.size());

		for (const auto& file : files)
		{
			FramePtr ptr = new (std::nothrow) Frame;
			if (ptr)
			{
				if (ptr->Load(file))
				{
					frames.push_back(ptr);
				}
			}
		}

		if (!frames.empty())
		{
			FrameSequencePtr fs = new (std::nothrow) FrameSequence(frames);
			return AddFrameSequence(id, fs);
		}
		return 0;
	}

	UInt32 ResourceCache::AddFrameSequence(String const & id, String const& file_path, Int32 cols, Int32 rows, Float32 padding_x, Float32 padding_y)
	{
		if (cols <= 0 || rows <= 0)
			return 0;

		FramePtr raw = new (std::nothrow) Frame;
		if (!raw || !raw->Load(file_path))
			return false;

		Float32 raw_width = raw->GetWidth();
		Float32 raw_height = raw->GetHeight();
		Float32 width = (raw_width - (cols - 1) * padding_x) / cols;
		Float32 height = (raw_height - (rows - 1) * padding_y) / rows;

		Vector<FramePtr> frames;
		frames.reserve(rows * cols);

		Float32 dty = 0;
		for (Int32 i = 0; i < rows; i++)
		{
			Float32 dtx = 0;
			for (Int32 j = 0; j < cols; j++)
			{
				FramePtr ptr = new (std::nothrow) Frame(raw->GetTexture());
				if (ptr)
				{
					ptr->SetCropRect(Rect{ dtx, dty, dtx + width, dty + height });
					frames.push_back(ptr);
				}
				dtx += (width + padding_x);
			}
			dty += (height + padding_y);
		}

		FrameSequencePtr fs = new (std::nothrow) FrameSequence(frames);
		return AddFrameSequence(id, fs);
	}

	UInt32 ResourceCache::AddFrameSequence(String const & id, FrameSequencePtr frames)
	{
		if (frames)
		{
			cache_.insert(std::make_pair(id, frames));
			return frames->GetFrames().size();
		}
		return 0;
	}

	bool ResourceCache::AddObjectBase(String const& id, ObjectBasePtr obj)
	{
		if (obj)
		{
			cache_.insert(std::make_pair(id, obj));
			return true;
		}
		return false;
	}

	bool ResourceCache::AddGifImage(String const& id, GifImage const& gif)
	{
		if (gif.IsValid())
		{
			gif_cache_.insert(std::make_pair(id, gif));
			return true;
		}
		return false;
	}

	bool ResourceCache::AddGifImage(String const& id, String const& file_path)
	{
		GifImage gif;
		if (gif.Load(file_path))
		{
			return AddGifImage(id, gif);
		}
		return false;
	}

	bool ResourceCache::AddFontCollection(String const& id, FontCollection const& collection)
	{
		if (collection.IsValid())
		{
			font_collection_cache_.insert(std::make_pair(id, collection));
			return true;
		}
		return false;
	}

	FramePtr ResourceCache::GetFrame(String const & id) const
	{
		return Get<Frame>(id);
	}

	FrameSequencePtr ResourceCache::GetFrameSequence(String const & id) const
	{
		return Get<FrameSequence>(id);
	}

	GifImage ResourceCache::GetGifImage(String const& id) const
	{
		auto iter = gif_cache_.find(id);
		if (iter != gif_cache_.end())
			return iter->second;
		return GifImage();
	}

	FontCollection ResourceCache::GetFontCollection(String const& id) const
	{
		auto iter = font_collection_cache_.find(id);
		if (iter != font_collection_cache_.end())
			return iter->second;
		return FontCollection();
	}

	void ResourceCache::Delete(String const & id)
	{
		cache_.erase(id);
	}

	void ResourceCache::Clear()
	{
		cache_.clear();
	}

}

namespace kiwano
{
	namespace __resource_cache_01
	{
		struct GlobalData
		{
			String path;
		};

		bool LoadTexturesFromData(ResourceCache* loader, GlobalData* gdata, const String* id, const String* type, const String* file)
		{
			if (!gdata || !id) return false;

			if (type && (*type) == L"gif")
			{
				// GIF image
				return loader->AddGifImage(*id, gdata->path + (*file));
			}

			if (file && !(*file).empty())
			{
				// Simple image
				return loader->AddFrame(*id, gdata->path + (*file));
			}
			return false;
		}

		bool LoadTexturesFromData(ResourceCache* loader, GlobalData* gdata, const String* id, const Vector<const WChar*>* files)
		{
			if (!gdata || !id) return false;

			// Frames
			if (files)
			{
				Vector<FramePtr> frames;
				frames.reserve(files->size());
				for (const auto& file : (*files))
				{
					FramePtr frame = new Frame;
					if (frame->Load(gdata->path + (file)))
					{
						frames.push_back(frame);
					}
				}
				FrameSequencePtr frame_seq = new FrameSequence(frames);
				return !!loader->AddFrameSequence(*id, frame_seq);
			}
			return false;
		}

		bool LoadTexturesFromData(ResourceCache* loader, GlobalData* gdata, const String* id, const String* file,
			Int32 rows, Int32 cols, Float32 padding_x, Float32 padding_y)
		{
			if (!gdata || !id) return false;

			if (file)
			{
				if (!(*file).empty())
				{
					if (rows || cols)
					{
						// Frame slices
						return !!loader->AddFrameSequence(*id, gdata->path + (*file), std::max(cols, 1), std::max(rows, 1), padding_x, padding_y);
					}
					else
					{
						// Simple image
						return loader->AddFrame(*id, gdata->path + (*file));
					}
				}
			}
			return false;
		}

		bool LoadFontsFromData(ResourceCache* loader, GlobalData* gdata, const String* id, const Vector<String>* files)
		{
			if (!gdata || !id) return false;

			// Font Collection
			if (files)
			{
				Vector<String> files_copy(*files);
				for (auto& file : files_copy)
				{
					file = gdata->path + file;
				}

				FontCollection collection;
				if (collection.Load(files_copy))
				{
					return loader->AddFontCollection(*id, collection);
				}
			}
			return false;
		}

		bool LoadJsonData(ResourceCache* loader, Json const& json_data)
		{
			GlobalData global_data;
			if (json_data.count(L"path"))
			{
				global_data.path = json_data[L"path"];
			}

			if (json_data.count(L"images"))
			{
				for (const auto& image : json_data[L"images"])
				{
					const String* id = nullptr, * type = nullptr, * file = nullptr;
					Int32 rows = 0, cols = 0;

					if (image.count(L"id")) id = &image[L"id"].as_string();
					if (image.count(L"type")) type = &image[L"type"].as_string();
					if (image.count(L"file")) file = &image[L"file"].as_string();
					if (image.count(L"rows")) rows = image[L"rows"].as_int();
					if (image.count(L"cols")) cols = image[L"cols"].as_int();

					if (rows || cols)
					{
						Float32 padding_x = 0, padding_y = 0;
						if (image.count(L"padding-x")) padding_x = image[L"padding-x"].get<Float32>();
						if (image.count(L"padding-y")) padding_y = image[L"padding-y"].get<Float32>();

						if (!LoadTexturesFromData(loader, &global_data, id, file, rows, cols, padding_x, padding_y))
							return false;
					}

					if (image.count(L"files"))
					{
						Vector<const WChar*> files;
						files.reserve(image[L"files"].size());
						for (const auto& file : image[L"files"])
						{
							files.push_back(file.as_string().c_str());
						}
						if(!LoadTexturesFromData(loader, &global_data, id, &files))
							return false;
					}
					else
					{
						if(!LoadTexturesFromData(loader, &global_data, id, type, file))
							return false;
					}
				}
			}

			if (json_data.count(L"fonts"))
			{
				for (const auto& font : json_data[L"fonts"])
				{
					String id;

					if (font.count(L"id")) id = font[L"id"].as_string();
					if (font.count(L"files"))
					{
						Vector<String> files;
						files.reserve(font[L"files"].size());
						for (const auto& file : font[L"files"])
						{
							files.push_back(file.as_string());
						}
						if (!LoadFontsFromData(loader, &global_data, &id, &files))
							return false;
					}
				}
			}
			return true;
		}

		bool LoadXmlData(ResourceCache* loader, const tinyxml2::XMLElement* elem)
		{
			GlobalData global_data;
			if (auto path = elem->FirstChildElement(L"path"))
			{
				global_data.path = path->GetText();
			}

			if (auto images = elem->FirstChildElement(L"images"))
			{
				for (auto image = images->FirstChildElement(); image; image = image->NextSiblingElement())
				{
					String id, type, file;
					Int32 rows = 0, cols = 0;

					if (auto attr = image->Attribute(L"id"))      id.assign(attr); // assign() copies attr content
					if (auto attr = image->Attribute(L"type"))    type = attr;     // operator=() just holds attr pointer
					if (auto attr = image->Attribute(L"file"))    file = attr;
					if (auto attr = image->IntAttribute(L"rows")) rows = attr;
					if (auto attr = image->IntAttribute(L"cols")) cols = attr;

					if (rows || cols)
					{
						Float32 padding_x = 0, padding_y = 0;
						if (auto attr = image->FloatAttribute(L"padding-x")) padding_x = attr;
						if (auto attr = image->FloatAttribute(L"padding-y")) padding_y = attr;

						if (!LoadTexturesFromData(loader, &global_data, &id, &file, rows, cols, padding_x, padding_y))
							return false;
					}

					if (file.empty() && !image->NoChildren())
					{
						Vector<const WChar*> files_arr;
						for (auto file = image->FirstChildElement(); file; file = file->NextSiblingElement())
						{
							if (auto path = file->Attribute(L"path"))
							{
								files_arr.push_back(path);
							}
						}
						if (!LoadTexturesFromData(loader, &global_data, &id, &files_arr))
							return false;
					}
					else
					{
						if (!LoadTexturesFromData(loader, &global_data, &id, &type, &file))
							return false;
					}
				}
			}

			if (auto fonts = elem->FirstChildElement(L"fonts"))
			{
				for (auto font = fonts->FirstChildElement(); font; font = font->NextSiblingElement())
				{
					String id;
					if (auto attr = font->Attribute(L"id")) id.assign(attr);

					Vector<String> files_arr;
					for (auto file = font->FirstChildElement(); file; file = file->NextSiblingElement())
					{
						if (auto path = file->Attribute(L"path"))
						{
							files_arr.push_back(path);
						}
					}
					if (!LoadFontsFromData(loader, &global_data, &id, &files_arr))
						return false;
				}
			}
			return true;
		}
	}
}