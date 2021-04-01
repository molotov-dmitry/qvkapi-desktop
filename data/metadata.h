/*!
 * \file
 * \brief Заголовочный файл с описанием функций преобразования типов данных
 */

#ifndef METADATA_H
#define METADATA_H

#include <QString>
#include <QDateTime>

/*!
 * \brief Класс для преобразования типов данных
 */
class Metadata
{
private:
    Metadata() {}

public:

    enum TimeUnit
    {
        TU_SECONDS,
        TU_MINUTES,
        TU_HOURS
    };

    enum PageType
    {
        PAGE_UNKNOWN,

        PAGE_USER,
        PAGE_ALBUM_LIST,
        PAGE_USER_PHOTOS,
        PAGE_ALBUM,
    };

    /*!
     * \brief Получение окончания единицы измерения времени
     * \param[in] value Количество времени
     * \param[in] timeUnit Единицы измерения времени
     * \return Окончание единицы измерения времени
     */
    static QString getSuffix(qint64 value, TimeUnit timeUnit = TU_MINUTES);

    /*!
     * \brief Преобразование разницы между подаваемым и текущим временем в строку в читаемом человеком формате
     * \param[in] dateTime Дата/время
     * \return Разница между подаваемым и текущим временем в читаемом человеком формате
     */
    static QString timeDiffStr(const QDateTime &dateTime);

    /*!
     * \brief Преобразование разницы между подаваемой и текущей датой в строку в читаемом человеком формате
     * \param dateTime Дата
     * \return Разница между подаваемой и текущей датой в читаемом человеком формате
     */
    static QString dateDiffStr(const QDateTime &dateTime);

    /*!
     * \brief Проверка корректности номера телефона
     * \param[in] phoneNumber Номер телефона
     * \return Корректность номера телефона
     */
    static bool checkPhoneNumber(const QString &phoneNumber);

    /*!
     * \brief Преобразование телефонного номера в более читаемый вид
     * \param[in] phoneNumber Номер телефона
     * \return Номер телефона в читаемом виде
     */
    static QString phoneNumberToString(const QString &phoneNumber);

    /*!
     * \brief Получение HTML тега ссылки
     * \param[in] linkUrl Адресс ссылки
     * \param[in] linkName Название ссылки
     * \return HTML тег ссылки
     */
    static QString getLink(const QString &linkUrl, const QString &linkName);

    /*!
     * \brief Преобразование URL в HTML-ссылку
     * \param[in] siteUrl URL
     * \return HTML-ссылка
     */
    static QString urlToLink(const QString &siteUrl);

    /*!
     * \brief Преобразование имени Skype в ссылку
     * \param[in] skypeName Имя пользователя Skype
     * \return Ссылка
     */
    static QString skypeToLink(const QString &skypeName);

    /*!
     * \brief Преобразование идентификатора и имени пользователя Facebook в ссылку на страницу
     * \param[in] facebookId Идентификатор пользователя
     * \param[in] facebookName Имя пользователя
     * \return Ссылка на страницу пользователя
     */
    static QString facebookToLink(const QString &facebookId, const QString &facebookName);

    /*!
     * \brief Преобразование имени пользователя Twitter в ссылку на страницу
     * \param[in] twitterId Имя пользователя
     * \return Ссылка на страницу пользователя
     */
    static QString twitterToLink(const QString &twitterName);

    /*!
     * \brief Преобразование имени пользователя Instagram в ссылку на страницу
     * \param[in] instagramName Имя пользователя
     * \return Ссылка на страницу пользователя
     */
    static QString instagramToLink(const QString &instagramName);

    /*!
     * \brief Определение типа страницы по адрусу страницы
     * \param[in] pageUri Адрес страницы
     * \return Тип страницы
     */
    static PageType getPageType(const QString &pageUri);
};

#endif // METADATA_H
