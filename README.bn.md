# ওপেনবাংলা কিবোর্ড (OBK)

ওপেনবাংলা কিবোর্ড হলো গ্নু/লিনাক্স (GNU/Linux), ম্যাকওএস (macOS) এবং উইন্ডোজ (Windows)-এর জন্য রাস্ট (Rust) ও সি++ (C++) দিয়ে তৈরি একটি উন্মুক্ত এবং ইউনিকোড-সমর্থিত বাংলা লেখার কিবোর্ড সফটওয়্যার। গতিশীল লেখার উদ্দেশ্যে এতে বিভিন্ন স্বয়ংক্রিয় টাইপিং ব্যবস্থা রয়েছে এবং এটি অভ্র ফোনেটিক, প্রভাত, মুনীর অপ্টিমা, এবং জাতীয় (Jatiya)-র মতো সুপ্রচলিত লেআউটগুলো সমর্থন করে।

[অভ্র কিবোর্ডের](https://github.com/mugli/Avro-Keyboard) অধিকাংশ ফিচার এখানে অন্তর্ভুক্ত করা হয়েছে, যা অভ্রর পুরোনো ব্যবহারকারীদের জন্য অত্যন্ত পরিচিত ও স্বাচ্ছন্দ্যদায়ক।

## ইনস্টলেশন

> [!IMPORTANT]
> আপনার সিস্টেমে ওপেনবাংলা কিবোর্ডের ২.০.০ বা পূর্ববর্তী কোনো সংস্করণ ইনস্টল করা থাকলে, অনুগ্রহ করে প্রথমে সেটি [আনইনস্টল](https://openbangla.github.io/manual#uninstall) করুন।

### দ্রুত শুরু

সর্বশেষ সংস্করণটি স্বয়ংক্রিয়ভাবে ইনস্টল করতে আপনার অপারেটিং সিস্টেম অনুযায়ী নিচের কমান্ডটি রান করুন।

**গ্নু/লিনাক্স এবং ম্যাকওএস (GNU/Linux and macOS)**

```sh
curl -fsSL https://openbangla.github.io/install | bash
```

**উইন্ডোজ (PowerShell)**

```powershell
irm "https://openbangla.github.io/install.ps1" | iex
```

### ম্যানুয়াল ইনস্টলেশন

যাঁরা সোর্স কোড থেকে বিল্ড করতে বা ম্যানুয়ালি এনভায়রনমেন্ট ভ্যারিয়েবল কনফিগার করতে পছন্দ করেন, তাঁরা অনুগ্রহ করে আমাদের [ম্যানুয়াল](https://openbangla.github.io/manual#installation) দেখুন। এছাড়া আপনার অপারেটিং সিস্টেমের জন্য রিলিজ পেজ থেকে সরাসরি ইনস্টলার (যেমন `.exe`, `.deb`) ডাউনলোড করে নিতে পারেন আমাদের [রিলিজ](https://github.com/OpenBangla/OpenBangla-Keyboard/releases) পেজ থেকে।

## কমিউনিটি

আমরা [গিটহাব ডিসকাশন](https://github.com/OpenBangla/OpenBangla-Keyboard/discussions) এবং [ফেসবুক](https://www.facebook.com/openbanglakeyboard)-এ আছি।

## অবদান (Contributing)

আমরা যেকোনো অবদানে স্বাগত জানাই! আমাদের অবদান প্রক্রিয়া সম্পর্কিত বিস্তারিত তথ্যের জন্য [CONTRIBUTING.md](CONTRIBUTING.md) দেখুন।

দ্রুত শুরু করতে:

1. রিপোজিটরিটি ফর্ক করুন।
2. নতুন একটি ব্রাঞ্চ তৈরি করুন (`git checkout -b feature-branch`)।
3. আপনার পরিবর্তনসমূহ কমিট করুন (`git commit -m 'Add new feature'`)।
4. ব্রাঞ্চটিতে পুশ করুন (`git push origin feature-branch`)।
5. একটি পুল রিকুয়েস্ট (Pull Request) ওপেন করুন।

যদি কোনো বাগ খুঁজে পান বা নতুন কোনো ফিচারের অনুরোধ থাকে, তবে অনুগ্রহ করে [একটি ইস্যু ওপেন করুন](https://github.com/OpenBangla/OpenBangla-Keyboard/issues)।

## কৃতজ্ঞতা (Acknowledgements)

- [মেহেদী হাসান খান](https://github.com/mugli), মূল অভ্র কিবোর্ড ডেভলপ ও রক্ষণাবেক্ষণ করার জন্য।
- [রীফাত নবী](https://github.com/torifat), অভ্র ফোনেটিক জাভাস্ক্রিপ্টে স্থানান্তর করার জন্য।
- [সারিম খান](https://github.com/sarim), আইবাস-অভ্র ডেভলপ করার জন্য।
- [Dec_32](https://github.com/dec32), [Ajemi](https://github.com/dec32/Ajemi) প্রজেক্ট ডেভলপ করার জন্য।
- [সাইফুল ইসলাম](https://github.com/saaiful), আইকনের জন্য।
- [Material Design Icons](https://material.io/resources/icons)।
- [QSimpleUpdater](https://github.com/alex-spataru/QSimpleUpdater), হালনাগাদকরণ লাইব্রেরির জন্য।

## লাইসেন্স

এই প্রজেক্টটি [GPL 3 License](https://opensource.org/licenses/GPL-3.0)-এর অধীনে লাইসেন্সকৃত।

[মুহাম্মদ মোমিনুল হক](https://github.com/mominul) এবং [✨ অবদানকারীগণের ✨](https://github.com/OpenBangla/OpenBangla-Keyboard/graphs/contributors) দ্বারা ভালোবাসার সাথে তৈরি!
